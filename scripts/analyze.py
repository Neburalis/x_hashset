#!/usr/bin/env python3
"""
Analyze x_hashset benchmark results.

Reads CSVs from data/, writes plots to assets/, prints summary table.

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
from collections import defaultdict

DATA_DIR   = "data"
ASSETS_DIR = "assets"


# -- helpers --

def _parse_hist_name(path):
    """hist_first_char_997.csv  ->  ('first_char', 997)"""
    base = os.path.splitext(os.path.basename(path))[0]  # hist_first_char_997
    rest = base[5:]                                       # first_char_997
    idx  = rest.rfind("_")
    return rest[:idx], int(rest[idx + 1:])


def _percentile(data, p):
    """p-th percentile (0-100) without numpy."""
    s = sorted(data)
    return s[min(int(p / 100 * len(s)), len(s) - 1)]


# -- Part 1: bucket distribution histograms --

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
               label=f"mean = {avg:.1f}\nσ/√avg = {norm:.2f}")
    # ax.axhspan(max(avg - sigma, 0), avg + sigma,
            #    color="#cc3333", alpha=0.10)
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


# -- Part 2: timing histograms --

def plot_timing_hist(func, size, dts):
    # clip outliers to p99 so the axis isn't squashed by interrupt spikes
    p99    = _percentile(dts, 99)
    clipped = [d for d in dts if d <= p99]

    n      = len(dts)
    # use clipped data for mean/sigma so they match what's shown on the plot
    mean   = sum(clipped) / len(clipped)
    sigma  = math.sqrt(sum((d - mean) ** 2 for d in clipped) / len(clipped))

    fig, ax = plt.subplots(figsize=(12, 5))
    ax.hist(clipped, bins=80, color="#2e8b57", edgecolor="none", density=True)
    ax.axvline(mean, color="#cc3333", linewidth=1.5, linestyle="--",
               label=f"mean = {mean:.0f} TSC\nsigma = {sigma:.0f}")
    # ax.axvspan(max(mean - sigma, 0), mean + sigma,
            #    color="#cc3333", alpha=0.10)
    ax.set_xlabel("TSC cycles per contains()", labelpad=8)
    ax.set_ylabel("Density", labelpad=8)
    ax.set_title(
        f"Search time distribution: {func}   [{size} buckets]   "
        f"n={n}  (clipped to p99={p99})"
    )
    ax.legend(loc="upper right")
    fig.tight_layout()

    out = os.path.join(ASSETS_DIR, f"timing_{func}_{size}.png")
    fig.savefig(out, dpi=150, facecolor=fig.get_facecolor())
    plt.close(fig)
    print(f"  {out}")


# -- summary table --

def print_table(rows):
    W = [12, 6, 10, 10, 10, 10]
    hdrs = ["func", "size", "mean TSC", "sigma", "min TSC", "max TSC"]
    seps = ["-" * w for w in W]

    def fmt(vals):
        parts = []
        for i, v in enumerate(vals):
            s = str(v)
            parts.append(s.ljust(W[i]) if i == 0 else s.rjust(W[i]))
        return "  ".join(parts)

    print(fmt(hdrs))
    print(fmt(seps))
    for func, size, dts in rows:
        n    = len(dts)
        mean = sum(dts) / n
        var  = sum((d - mean) ** 2 for d in dts) / n
        sig  = math.sqrt(var)
        print(fmt([func, size, f"{mean:.1f}", f"{sig:.1f}", min(dts), max(dts)]))


# -- main --

def main():
    os.makedirs(DATA_DIR,   exist_ok=True)
    os.makedirs(ASSETS_DIR, exist_ok=True)

    # bucket distribution histograms
    print("=== Bucket distribution histograms ===")
    hist_files = sorted(glob.glob(os.path.join(DATA_DIR, "hist_*.csv")))
    if not hist_files:
        print(f"  (no {DATA_DIR}/hist_*.csv found - run ./hashset.out first)")
    for path in hist_files:
        plot_bucket_hist(path)

    # timing histograms + table
    timing_csv = os.path.join(DATA_DIR, "timing.csv")
    if not os.path.exists(timing_csv):
        print(f"\n{timing_csv} not found - run ./hashset.out first.")
        return

    print("\n=== Search timing histograms ===")
    groups = defaultdict(list)
    with open(timing_csv, newline="") as f:
        for row in csv.DictReader(f):
            try:
                groups[(row["func"], int(row["size"]))].append(int(row["dt"]))
            except (TypeError, ValueError):
                continue  # skip malformed rows

    ordered = list(groups.items())   # insertion order = order in all_funcs[]
    for (func, size), dts in ordered:
        plot_timing_hist(func, size, dts)

    print("\n=== Performance summary ===\n")
    print_table([(func, size, dts) for (func, size), dts in ordered])


if __name__ == "__main__":
    main()
