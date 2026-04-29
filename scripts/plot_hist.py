#!/usr/bin/env python3
"""
Plot a per-bucket load histogram for x_hashset.

Usage:
    python3 scripts/plot_hist.py <csv_path> <hash_name> <output_png>

CSV format (produced by x_hashset::dump_hist):
    bucket,count
    0,3
    1,7
    ...
"""

import sys
import csv
import matplotlib
matplotlib.use("Agg")           # no display needed
import matplotlib.pyplot as plt


def main():
    if len(sys.argv) != 4:
        print(f"Usage: {sys.argv[0]} <csv_path> <hash_name> <output_png>",
              file=sys.stderr)
        sys.exit(1)

    csv_path, hash_name, output_path = sys.argv[1], sys.argv[2], sys.argv[3]

    buckets, counts = [], []
    with open(csv_path, newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            buckets.append(int(row["bucket"]))
            counts.append(int(row["count"]))

    total   = sum(counts)
    nonzero = sum(1 for c in counts if c > 0)
    avg     = total / len(counts) if counts else 0

    with plt.style.context("dark_background"):
        fig, ax = plt.subplots(figsize=(14, 6))

        ax.bar(buckets, counts,
               width=1.0,
               color="#4488ff",
               edgecolor="none",
               linewidth=0)

        ax.axhline(avg, color="#ff6644", linewidth=1.2, linestyle="--",
                   label=f"avg = {avg:.1f}")

        ax.set_xlabel("Bucket index", labelpad=8)
        ax.set_ylabel("Elements count", labelpad=8)
        ax.set_title(f"Hash function: {hash_name}   "
                     f"[total={total}  non-empty={nonzero}/{len(counts)}]")

        ax.set_xlim(-0.5, len(buckets) - 0.5)
        ax.legend(loc="upper right")

        for spine in ax.spines.values():
            spine.set_edgecolor("#333333")

        fig.tight_layout()
        fig.savefig(output_path, dpi=150, facecolor=fig.get_facecolor())

    print(f"Saved histogram to {output_path}")


if __name__ == "__main__":
    main()
