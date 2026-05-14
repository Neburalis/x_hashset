#!/usr/bin/env -S uv run --script

# /// script
# requires-python = ">=3.10"
# dependencies = [
#   "matplotlib",
# ]
# ///
"""
Build bucket-distribution histograms from data/hist_*.csv.

Usage:
    python3 scripts/analyze.py
"""

import os
import glob
import csv
import math
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

DATA_DIR   = "data"
ASSETS_DIR = "assets/distribution_hist"


def _parse_hist_name(path):
    """hist_first_char_997.csv  ->  ('first_char', 997)"""
    base = os.path.splitext(os.path.basename(path))[0]  # hist_first_char_997
    rest = base[5:]                                       # first_char_997
    idx  = rest.rfind("_")
    return rest[:idx], int(rest[idx + 1:])


def plot_bucket_hist(csv_path):
    func, size = _parse_hist_name(csv_path)

    buckets, counts = [], []
    with open(csv_path, newline="") as f:
        for row in csv.DictReader(f):
            buckets.append(int(row["bucket"]))
            counts.append(int(row["count"]))

    total   = sum(counts)
    nonzero = sum(1 for c in counts if c > 0)
    avg     = total / len(counts) if counts else 0
    var     = sum((c - avg) ** 2 for c in counts) / len(counts) if counts else 0
    sigma   = math.sqrt(var)
    norm    = sigma / math.sqrt(avg) if avg > 0 else float('inf')

    fig, ax = plt.subplots(figsize=(14, 6))
    ax.bar(buckets, counts, width=1.0, color="#3a6bc4", edgecolor="none")
    ax.axhline(avg, color="#cc3333", linewidth=1.2, linestyle="--",
               label=f"mean = {avg:.1f}\ns/sqrt(avg) = {norm:.2f}")
    ax.set_xlabel("Bucket index", labelpad=8)
    ax.set_ylabel("Elements count", labelpad=8)
    ax.set_title(
        f"Bucket distribution: {func}   [{size} buckets]   "
        f"total={total}  non-empty={nonzero}/{len(counts)}"
    )
    ax.set_xlim(-0.5, len(buckets) - 0.5)
    ax.legend(loc="upper right")
    fig.tight_layout()

    out = os.path.join(ASSETS_DIR, f"hist_{func}_{size}.png")
    fig.savefig(out, dpi=150, facecolor=fig.get_facecolor())
    plt.close(fig)
    print(f"  {out}")


def main():
    os.makedirs(DATA_DIR,   exist_ok=True)
    os.makedirs(ASSETS_DIR, exist_ok=True)

    print("=== Bucket distribution histograms ===")
    hist_files = sorted(glob.glob(os.path.join(DATA_DIR, "hist_*.csv")))
    if not hist_files:
        print(f"  (no {DATA_DIR}/hist_*.csv found - run ./hashset.out first)")
        return
    for path in hist_files:
        plot_bucket_hist(path)


if __name__ == "__main__":
    main()
