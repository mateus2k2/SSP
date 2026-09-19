#!/usr/bin/env python3
"""Validate every mainCpp solution report under one or more folders.

Each report is checked against its instance for every SSP-USPrC constraint
(tools in the magazine, magazine capacity, no switches in unsupervised time,
planning horizon, no overlaps, reentrant strong chain, ...) and its footer
counters and objective are recomputed from the printed schedule. See
ssp/validation.py for the full list of checks.

Reports are grouped by folder for the summary; numbered run folders (run-01,
1, 2, ...) are collapsed into their parent, so a 10-run PT experiment shows up
as one line.

Usage:
  python3 scripts/results/validateRuns.py output-final
  python3 scripts/results/validateRuns.py output-final/same-toolset --issues-csv issues.csv
  python3 scripts/results/validateRuns.py output/Beezao/teste.txt --costs beezao -v
"""
import argparse
import collections
import csv
import os
import re
import sys
from concurrent.futures import ProcessPoolExecutor

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))  # scripts/, for ssp
from ssp import validation as vd  # noqa: E402

RUN_DIR = re.compile(r"^(run-)?\d+$")


def collect(paths):
    files = []
    for p in paths:
        if os.path.isfile(p):
            files.append(p)
            continue
        for root, dirs, names in os.walk(p):
            dirs.sort()
            for n in sorted(names):
                f = os.path.join(root, n)
                if not n.endswith((".sol", ".lp", ".md")) and vd.is_report(f):
                    files.append(f)
    return files


def group_of(path, base):
    rel = os.path.relpath(os.path.dirname(os.path.abspath(path)), os.path.abspath(base))
    parts = [x for x in rel.split(os.sep) if x not in (".", "") and not RUN_DIR.match(x)]
    return "/".join(parts) or "."


def _run(args):
    path, costs = args
    return vd.validate_report(path, costs)


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("paths", nargs="+", help="report files or folders (searched recursively)")
    ap.add_argument("--costs", default="auto", choices=["auto", *vd.COST_PRESETS],
                    help="objective weights (auto: 'beezao' for .PMTC instances, 'default' otherwise)")
    ap.add_argument("--issues-csv", help="write every issue found to this CSV")
    ap.add_argument("--jobs", type=int, default=os.cpu_count(), help="parallel workers")
    ap.add_argument("-v", "--verbose", action="store_true", help="print each issue")
    args = ap.parse_args(argv)

    files = collect(args.paths)
    if not files:
        sys.exit("no reports found")
    base = args.paths[0] if len(args.paths) == 1 and os.path.isdir(args.paths[0]) else os.getcwd()
    with ProcessPoolExecutor(max_workers=args.jobs) as ex:
        results = list(ex.map(_run, [(f, args.costs) for f in files], chunksize=16))

    groups = collections.OrderedDict()
    for r in results:
        groups.setdefault(group_of(r.report, base), []).append(r)

    print(f"{len(results)} reports, {sum(not r.ok for r in results)} with errors\n")
    for g, rs in groups.items():
        bad = sum(not r.ok for r in rs)
        methods = ",".join(sorted({r.method for r in rs}))
        print(f"{g}  [{methods}]  {len(rs)} reports, {bad} with errors")
        counts = collections.Counter()
        affected = collections.Counter()
        for r in rs:
            seen = set()
            for i in r.issues:
                counts[(i.severity, i.check)] += 1
                seen.add((i.severity, i.check))
            affected.update(seen)
        for (sev, check), n in sorted(counts.items()):
            print(f"    {sev:7} {check:28} {affected[(sev, check)]:5} reports, {n:6} occurrences  - {vd.CHECKS[check]}")
        if args.verbose:
            for r in rs:
                for i in r.issues:
                    where = "" if i.machine is None else f" m{i.machine}"
                    where += "" if i.job is None else f" job {i.job}" + ("" if i.op is None else f" op {i.op}")
                    print(f"      {os.path.relpath(r.report)}:{where} {i.check}: {i.message}")
        print()

    if args.issues_csv:
        with open(args.issues_csv, "w", newline="") as f:
            w = csv.writer(f)
            w.writerow(["group", "report", "instance", "method", "costs", "severity", "check",
                        "machine", "job", "op", "message"])
            for g, rs in groups.items():
                for r in rs:
                    for i in r.issues:
                        w.writerow([g, os.path.relpath(r.report), r.instance, r.method, r.costs,
                                    i.severity, i.check, i.machine, i.job, i.op, i.message])
        print(f"issues written to {args.issues_csv}")
    sys.exit(1 if any(not r.ok for r in results) else 0)


if __name__ == "__main__":
    main()
