#!/usr/bin/env python3
"""Print the magazine trace of one mainCpp report, to inspect KTNS decisions.

For every operation, in machine order: the tools it needs, the magazine while
it runs, and how many magazine tools no later operation on that machine needs
(tools KTNS keeps loaded for nothing).

Usage:
  python3 scripts/results/ktnsTrace.py output-final/same-toolset/Practitioner-single-run/n=15,p=0.5,r=0.5,t=0,v0.csv
"""
import argparse
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))  # scripts/, for ssp
from ssp import instances  # noqa: E402
from ssp import reports  # noqa: E402


def trace(path):
    plan, machines, _ = reports.parseReport(path)
    inst = instances.load_instance(plan["jobsFileName"])
    for m, ops in enumerate(machines):
        print(f"Machine {m}")
        print(f"  {'job':>5} {'op':>2} {'start':>7} {'end':>7} {'needs':>5} {'magazine':>8} {'never needed again':>18}")
        for i, o in enumerate(ops):
            spec = inst.ops.get((o["job"], o["operation"]))
            later = set().union(*(inst.ops[(x["job"], x["operation"])].tools
                                  for x in ops[i:] if (x["job"], x["operation"]) in inst.ops))
            idle = set(o["magazine"]) - later
            needs = len(spec.tools) if spec else "?"
            print(f"  {o['job']:>5} {o['operation']:>2} {o['start']:>7} {o['end']:>7} {needs:>5} "
                  f"{len(o['magazine']):>8} {len(idle):>18}")
        print()


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("report")
    trace(ap.parse_args().report)


if __name__ == "__main__":
    main()
