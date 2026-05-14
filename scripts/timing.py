#!/usr/bin/env -S uv run --script

# /// script
# requires-python = ">=3.10"
# dependencies = []
# ///
"""
Read wall-clock timing data from data/wall_*.csv and write a markdown
table into the <!-- timing-table --> section of README.md.

Usage:
    python3 scripts/timing.py
"""

import os
import glob
import csv

DATA_DIR = "data"
README   = "README.md"
MARKER   = "<!-- timing-table -->"

ORDER = ["base_verify", "base", "soa", "soa_hw", "aligned"]


def median(xs):
    s = sorted(xs)
    n = len(s)
    if not s:
        return 0.0
    return s[n // 2] if n % 2 else (s[n // 2 - 1] + s[n // 2]) / 2


def load_csv(path):
    times = []
    with open(path, newline="") as f:
        for row in csv.DictReader(f):
            try:
                times.append(float(row["rep_ms"]))
            except (TypeError, ValueError):
                continue
    return times


def sigma(xs):
    n = len(xs)
    if n < 2:
        return 0.0
    mean = sum(xs) / n
    return (sum((x - mean) ** 2 for x in xs) / (n - 1)) ** 0.5


def build_table(rows):
    lines = []
    lines.append("| variant | reps | median rep ms | sigma ms | min rep ms | max rep ms |")
    lines.append("|---|---|---|---|---|---|")
    for variant, ts in rows:
        n = len(ts)
        lines.append(
            f"| `{variant}` | {n} "
            f"| {median(ts):.1f} "
            f"| {sigma(ts):.1f} "
            f"| {min(ts):.1f} "
            f"| {max(ts):.1f} |"
        )
    return "\n".join(lines)


def main():
    data = {}
    for path in glob.glob(os.path.join(DATA_DIR, "wall_*.csv")):
        variant = os.path.splitext(os.path.basename(path))[0][len("wall_"):]
        ts = load_csv(path)
        if ts:
            data[variant] = ts

    if not data:
        print(f"No {DATA_DIR}/wall_*.csv found.")
        print("Run: make perf_base_verify / perf_base / perf_soa / perf_soa_hw / perf_aligned")
        return

    ordered = sorted(data.keys(),
                     key=lambda v: (ORDER.index(v) if v in ORDER else 999, v))
    rows = [(v, data[v]) for v in ordered]

    table = build_table(rows)
    print(table)
    print()

    if not os.path.exists(README):
        print(f"{README} not found, skipping update.")
        return

    with open(README, "r") as f:
        content = f.read()

    pos = content.find(MARKER)
    if pos == -1:
        print(f"Marker '{MARKER}' not found in {README}.")
        print("Add it to mark where the table should appear.")
        return

    after_marker = content[pos + len(MARKER):]
    next_heading  = after_marker.find("\n#")
    if next_heading == -1:
        tail = ""
    else:
        tail = after_marker[next_heading:]

    new_content = content[:pos + len(MARKER)] + "\n\n" + table + "\n" + tail
    with open(README, "w") as f:
        f.write(new_content)
    print(f"Updated {README}")


if __name__ == "__main__":
    main()
