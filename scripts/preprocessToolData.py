#!/usr/bin/env python3
"""
Rebuilds derived tool/job data files under input/Processed/ from the files
that are still authoritative source data (ToolSetInt.csv, 250.csv, 750.csv,
1000.csv). These outputs used to be committed directly with no way to
regenerate them; this script closes that gap.

  all-jobs-filtered   AllJobsFiltered.csv -- 250/750/1000.csv jobs with
                       dominated/duplicate-toolset/oversize jobs removed (a
                       subset-removal filter -- see remove_subsets() in
                       generateInstances.py for why this isn't perfectly
                       reproducible byte-for-byte across Python versions).
  unused-toolsets     UnusedToolSets.csv (every ToolSetInt.csv toolset NOT
                       referenced by any 250/750/1000.csv job) and, from
                       that, UnusedToolSetsClean.csv (further filtered to
                       drop toolsets that contain another one already in
                       the unused set, plus oversize/empty ones) -- this
                       is the file generateInstances.py's `same`/
                       `different`/`base` subcommands actually read.
  used-toolsets       UsedToolSet.csv -- the toolsets 250/750/1000.csv DO
                       reference, with their tool ids renumbered to a dense
                       range covering just those tools.

Ported from scripts/deprecated/filterJobs.py and filterToolSets.py, which
routed some of this (used-toolsets in particular) through a since-deleted
ToolSetOG.csv (raw, string-named tool data) even where the same result is
directly derivable from ToolSetInt.csv, which is already int-normalized --
verified by reproducing used-toolsets this way and confirming it matches
the toolset count and dense tool-id count of the file that used to be
committed here.

Usage:
  python3 scripts/preprocessToolData.py used-toolsets
  python3 scripts/preprocessToolData.py unused-toolsets
  python3 scripts/preprocessToolData.py all-jobs-filtered --out-dir /tmp/test
"""
import argparse
import csv
from pathlib import Path

from generateInstances import remove_subsets
from uteis.loadData import loadJobs, loadToolSet

REPO_ROOT = Path(__file__).resolve().parent.parent
PROCESSED_DIR = REPO_ROOT / "input" / "Processed"

JOB_FILES = ("250.csv", "750.csv", "1000.csv")
MAX_TOOLSET_SIZE = 80


def load_all_jobs():
    jobs = []
    for fname in JOB_FILES:
        jobs += loadJobs(str(PROCESSED_DIR / fname))
    return jobs


def write_toolset_csv(rows, out_path):
    """rows: list of (toolset_id, [tool, tool, ...]). Ragged-row ';'-delimited
    format matching ToolSetInt.csv/UnusedToolSetsClean.csv: id, then tools,
    then blank-padded to the widest row (same convention loadToolSet() reads)."""
    widest = max((len(tools) for _, tools in rows), default=0)
    out_path.parent.mkdir(parents=True, exist_ok=True)
    with open(out_path, "w", newline="") as f:
        writer = csv.writer(f, delimiter=";")
        for ts_id, tools in rows:
            writer.writerow([ts_id] + tools + [""] * (widest - len(tools)))


# ------------------------------------------------------------------------------------------------
# all-jobs-filtered
# ------------------------------------------------------------------------------------------------

def cmd_all_jobs_filtered(args):
    tool_sets = loadToolSet(str(PROCESSED_DIR / "ToolSetInt.csv"))

    per_file_filtered = []
    for fname in JOB_FILES:
        per_file_filtered += remove_subsets(tool_sets, loadJobs(str(PROCESSED_DIR / fname)))
    all_filtered = remove_subsets(tool_sets, per_file_filtered)

    out_path = args.out_dir / "AllJobsFiltered.csv" if args.out_dir else PROCESSED_DIR / "AllJobsFiltered.csv"
    fields = ["Job", "Operation", "ToolSet", "Processing Time"]
    out_path.parent.mkdir(parents=True, exist_ok=True)
    with open(out_path, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=fields, delimiter=";")
        w.writeheader()
        for job in all_filtered:
            w.writerow({k: job[k] for k in fields})

    print(f"Wrote {len(all_filtered)} jobs to {out_path}")


# ------------------------------------------------------------------------------------------------
# unused-toolsets
# ------------------------------------------------------------------------------------------------

def get_unused_toolsets(jobs, tool_sets):
    """Every toolset not referenced by any job. Ported from
    filterToolSets.py's getUnsuedToolSets (minus the incremental-file-write,
    which is now the caller's job)."""
    used_ids = {job['ToolSet'] for job in jobs}
    # size-descending, same order the original wrote/consumed
    return sorted(
        ((ts_id, tools) for ts_id, tools in tool_sets.items() if ts_id not in used_ids),
        key=lambda row: len(row[1]), reverse=True,
    )


def clean_unused_toolsets(jobs, unused_rows):
    """Drop any unused toolset that contains ANOTHER unused toolset (so only
    "minimal" ones remain), plus empty/oversize ones. Ported from
    filterToolSets.py's isSubSetOf + cleanUnsuedToolSets. O(n^2) -- ~3000
    unused toolsets takes well under a minute."""
    ids = [ts_id for ts_id, _ in unused_rows]
    sets = [tools for _, tools in unused_rows]
    as_sets = [set(t) for t in sets]

    kept = []
    for i in range(len(sets)):
        contains_another = False
        for j in range(i, len(sets)):
            if i == j or len(sets[j]) == 0:
                continue
            if sets[i] == sets[j] or as_sets[j].issubset(as_sets[i]):
                contains_another = True
                break
        if not contains_another and 0 < len(sets[i]) <= MAX_TOOLSET_SIZE:
            kept.append((ids[i], sets[i]))

    used_ids = {job['ToolSet'] for job in jobs}
    kept = [(ts_id, tools) for ts_id, tools in kept if ts_id not in used_ids]
    return kept


def cmd_unused_toolsets(args):
    jobs = load_all_jobs()
    tool_sets = loadToolSet(str(PROCESSED_DIR / "ToolSetInt.csv"))

    unused = get_unused_toolsets(jobs, tool_sets)
    out_dir = args.out_dir or PROCESSED_DIR
    write_toolset_csv(unused, out_dir / "UnusedToolSets.csv")
    print(f"Wrote {len(unused)} unused toolsets to {out_dir / 'UnusedToolSets.csv'}")

    cleaned = clean_unused_toolsets(jobs, unused)
    write_toolset_csv(cleaned, out_dir / "UnusedToolSetsClean.csv")
    print(f"Wrote {len(cleaned)} cleaned unused toolsets to {out_dir / 'UnusedToolSetsClean.csv'}")


# ------------------------------------------------------------------------------------------------
# used-toolsets
# ------------------------------------------------------------------------------------------------

def cmd_used_toolsets(args):
    jobs = load_all_jobs()
    tool_sets = loadToolSet(str(PROCESSED_DIR / "ToolSetInt.csv"))
    used_ids = {job['ToolSet'] for job in jobs}

    tool_map = {}
    rows = []
    for ts_id in sorted(used_ids):
        dense = []
        for tool in tool_sets[ts_id]:
            if tool not in tool_map:
                tool_map[tool] = len(tool_map) + 1
            dense.append(tool_map[tool])
        rows.append((ts_id, sorted(dense)))

    out_path = (args.out_dir or PROCESSED_DIR) / "UsedToolSet.csv"
    write_toolset_csv(rows, out_path)
    print(f"Wrote {len(rows)} used toolsets ({len(tool_map)} distinct tools) to {out_path}")


# ------------------------------------------------------------------------------------------------
# CLI
# ------------------------------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest="command", required=True)

    for name, func in (
        ("all-jobs-filtered", cmd_all_jobs_filtered),
        ("unused-toolsets", cmd_unused_toolsets),
        ("used-toolsets", cmd_used_toolsets),
    ):
        p = sub.add_parser(name)
        p.add_argument("--out-dir", type=Path, default=None,
                        help="Override the output directory (default: input/Processed/)")
        p.set_defaults(func=func)

    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
