#!/usr/bin/env python3
"""Print, per instance file in a folder, how many distinct tools it uses, its
magazine capacity, and how many operations need more tools than the magazine
holds (those can never be processed).

Reads every instance format (legacy CSV, BaseInstances, Consolidated, Beezao).

Usage:
  python3 scripts/instances/countTools.py input/MyInstancesSameToolSets
"""
import argparse
import os
import sys

from natsort import natsorted

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))  # scripts/, for ssp
from ssp import instances  # noqa: E402

EXTENSIONS = (".csv", ".txt", ".PMTC")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("folder")
    folder = ap.parse_args().folder

    print(f"{'instance':<45} {'tools':>6} {'capacity':>8} {'ops over capacity':>17}")
    for name in natsorted(os.listdir(folder)):
        if not name.endswith(EXTENSIONS):
            continue
        inst = instances.load_instance(os.path.join(folder, name))
        tools = set().union(*(o.tools for o in inst.ops.values()))
        over = sum(len(o.tools) > inst.capacity for o in inst.ops.values())
        print(f"{name:<45} {len(tools):>6} {inst.capacity:>8} {over:>17}")


if __name__ == "__main__":
    main()
