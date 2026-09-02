#!/usr/bin/env python3
"""
Generates SSP problem instances directly in the Consolidated single-file
format (see input/Consolidated/README and src/loadData.cpp's
SSP::loadInstanceConsolidated), under:

    input/Consolidated/SameToolSets/
    input/Consolidated/DiferentToolSets/

Two independent strategies, one per subcommand/output folder:

  same        Built from real job data (input/Processed/{250,750,1000}.csv).
              A reentrant job's two operations SHARE one toolset drawn from
              the real pool (input/Processed/ToolSetInt.csv) -- this sharing
              is what "SameToolSets" means.
  different   Synthetic instances where every operation gets its own,
              never-reused toolset, drawn from
              input/Processed/UnusedToolSetsClean.csv.

Both strategies vary instance size, priority level, and reentrant ratio.
Does NOT touch BeezaoRaw/ or BaseInstances/ -- those are separate, already
raw/pre-built instance formats.

Usage:
  python3 scripts/generateInstances.py same --sizes 15,25,50,75 --priority-levels 0.25,0.5,0.75
  python3 scripts/generateInstances.py different --sizes 15,25,50,75

All paths are relative to the repo root (resolved from this file's
location), so this runs the same regardless of which machine/checkout it's
run from.
"""
import argparse
import random
from pathlib import Path

from uteis.loadData import loadToolSet, loadJobs
from uteis.ProcessingTimeGenerator import ProcessingTimeGenerator

REPO_ROOT = Path(__file__).resolve().parent.parent
INPUT_DIR = REPO_ROOT / "input"
PROCESSED_DIR = INPUT_DIR / "Processed"
CONSOLIDATED_DIR = INPUT_DIR / "Consolidated"

DEFAULT_CAPACITY = 80
DEFAULT_MACHINES = 2
DEFAULT_DAYS = 2
DEFAULT_UNSUPERVISED_MINUTS = 720


# ------------------------------------------------------------------------------------------------
# CONSOLIDATED FORMAT WRITER
# ------------------------------------------------------------------------------------------------

def write_consolidated(jobs, tool_sets, meta, title, out_path):
    """jobs: list of {'Job','Operation','Priority','Processing Time','ToolSet'} dicts,
             where 'ToolSet' is a key into tool_sets.
       tool_sets: dict of toolset-id -> list of raw tool ids.
       meta: dict with CAPACITY/MACHINES/DAYS/UNSUPERVISED_MINUTS.
       title: informational first line (kept out of the filename; files are
              numbered so runs never collide on name).
    """
    normalized_tools = {}   # raw tool id -> dense 0-based index
    normalized_sets = {}    # toolset id -> dense index list, deduped once per toolset
    for job in jobs:
        ts_id = job['ToolSet']
        if ts_id in normalized_sets:
            continue
        dense = []
        for tool in tool_sets[ts_id]:
            if tool not in normalized_tools:
                normalized_tools[tool] = len(normalized_tools)
            dense.append(normalized_tools[tool])
        normalized_sets[ts_id] = sorted(dense)

    tool_count = len(normalized_tools)
    bool_rows = {}
    for ts_id, dense in normalized_sets.items():
        row = [0] * tool_count
        for idx in dense:
            row[idx] = 1
        bool_rows[ts_id] = row

    out_path.parent.mkdir(parents=True, exist_ok=True)
    with open(out_path, "w", newline='') as f:
        f.write(f"{title}\n")
        f.write(f"{meta['CAPACITY']}\n")
        f.write(f"{meta['MACHINES']}\n")
        f.write(f"{meta['DAYS']}\n")
        f.write(f"{meta['UNSUPERVISED_MINUTS']}\n\n")
        for job in jobs:
            row = bool_rows[job['ToolSet']]
            f.write(f"{job['Job']} {job['Operation']} {job['Priority']} {job['Processing Time']} ")
            f.write(" ".join(str(bit) for bit in row))
            f.write("\n")


def count_unique_tools(jobs, tool_sets):
    unique = set()
    for job in jobs:
        unique.update(tool_sets[job['ToolSet']])
    return len(unique)


# ------------------------------------------------------------------------------------------------
# SAME TOOLSETS  (reentrant job's 2 operations share one toolset)
# ------------------------------------------------------------------------------------------------

def make_same_toolset_instance(size, reentrant_ratio, priority_level, jobs_pool, time_gen, rng):
    chosen = rng.sample(jobs_pool, size)

    n_reentrant = int(size / (1 + reentrant_ratio) / 2)
    for i, job in enumerate(chosen):
        job = dict(job)  # don't mutate the shared pool
        chosen[i] = job
        job['Reentrant'] = i < n_reentrant

    rng.shuffle(chosen)
    priority_budget = int(size * priority_level)
    for job in chosen:
        if priority_budget > 0:
            job['Priority'] = 1
            priority_budget -= 2 if job['Reentrant'] else 1
        else:
            job['Priority'] = 0

    expanded = []
    for i, job in enumerate(chosen):
        if job['Reentrant']:
            t1, t2 = time_gen.generate_random_numbers(2)
            expanded.append({'Job': i, 'Operation': 0, 'Priority': job['Priority'],
                              'Processing Time': t1, 'ToolSet': job['ToolSet']})
            expanded.append({'Job': i, 'Operation': 1, 'Priority': job['Priority'],
                              'Processing Time': t2, 'ToolSet': job['ToolSet']})
        else:
            expanded.append({'Job': i, 'Operation': 0, 'Priority': job['Priority'],
                              'Processing Time': job['Processing Time'], 'ToolSet': job['ToolSet']})

    expanded.sort(key=lambda x: x['Job'])
    return expanded


def generate_same(args):
    rng = random.Random(args.seed)
    time_gen = ProcessingTimeGenerator()
    tool_sets = loadToolSet(str(PROCESSED_DIR / "ToolSetInt.csv"))

    jobs_pool = []
    for fname in ("250.csv", "750.csv", "1000.csv"):
        jobs_pool += loadJobs(str(PROCESSED_DIR / fname))

    meta = {"CAPACITY": args.capacity, "MACHINES": args.machines,
            "DAYS": args.days, "UNSUPERVISED_MINUTS": args.unsupervised}

    out_dir = args.out_dir or (CONSOLIDATED_DIR / "SameToolSets")
    count = 0
    for size in args.sizes:
        if size > len(jobs_pool):
            print(f"Skipping size={size}: only {len(jobs_pool)} real jobs available to draw from")
            continue
        for priority_level in args.priority_levels:
            jobs = make_same_toolset_instance(size, args.reentrant_ratio, priority_level, jobs_pool, time_gen, rng)
            unique_tools = count_unique_tools(jobs, tool_sets)
            title = f"n={size},p={priority_level:.2f},r={args.reentrant_ratio},t={unique_tools}"
            write_consolidated(jobs, tool_sets, meta, title, out_dir / f"{count}.txt")
            print(f"  [{count}] {title} -> {out_dir / f'{count}.txt'}")
            count += 1

    print(f"Wrote {count} instances to {out_dir}")


# ------------------------------------------------------------------------------------------------
# DIFFERENT TOOLSETS  (every operation gets its own, unique toolset)
# ------------------------------------------------------------------------------------------------

def make_different_toolset_instance(size, reentrant_ratio, priority_level, toolset_ids, time_gen, rng):
    n_reentrant = int(size / (1 + reentrant_ratio) / 2)
    needed = size + n_reentrant  # reentrant jobs consume 2 ids, non-reentrant consume 1
    if needed > len(toolset_ids):
        raise ValueError(f"need {needed} distinct toolsets for size={size}, only {len(toolset_ids)} available")

    jobs = []
    ts_i = 0
    for i in range(n_reentrant):
        jobs.append({'Job': i, 'Reentrant': True, 'ToolSets': [toolset_ids[ts_i], toolset_ids[ts_i + 1]]})
        ts_i += 2
    for i in range(n_reentrant, size):
        jobs.append({'Job': i, 'Reentrant': False, 'ToolSets': [toolset_ids[ts_i]]})
        ts_i += 1

    rng.shuffle(jobs)
    priority_budget = int(size * priority_level)
    for job in jobs:
        if priority_budget > 0:
            job['Priority'] = 1
            priority_budget -= 2 if job['Reentrant'] else 1
        else:
            job['Priority'] = 0

    expanded = []
    for i, job in enumerate(jobs):
        if job['Reentrant']:
            t1, t2 = time_gen.generate_random_numbers(2)
            expanded.append({'Job': i, 'Operation': 0, 'Priority': job['Priority'],
                              'Processing Time': t1, 'ToolSet': job['ToolSets'][0]})
            expanded.append({'Job': i, 'Operation': 1, 'Priority': job['Priority'],
                              'Processing Time': t2, 'ToolSet': job['ToolSets'][1]})
        else:
            t1 = time_gen.generate_random_numbers(1)[0]
            expanded.append({'Job': i, 'Operation': 0, 'Priority': job['Priority'],
                              'Processing Time': t1, 'ToolSet': job['ToolSets'][0]})

    expanded.sort(key=lambda x: x['Job'])
    return expanded


def generate_different(args):
    rng = random.Random(args.seed)
    time_gen = ProcessingTimeGenerator()
    tool_unused = loadToolSet(str(PROCESSED_DIR / "UnusedToolSetsClean.csv"))
    tool_pool = list(tool_unused.keys())

    meta = {"CAPACITY": args.capacity, "MACHINES": args.machines,
            "DAYS": args.days, "UNSUPERVISED_MINUTS": args.unsupervised}

    out_dir = args.out_dir or (CONSOLIDATED_DIR / "DiferentToolSets")
    count = 0
    for size in args.sizes:
        rng.shuffle(tool_pool)
        # generous upper bound (needed <= size + size/2 for any reentrant_ratio >= 1);
        # make_different_toolset_instance raises if this ever isn't enough.
        toolset_slice = tool_pool[:size * 2]
        for priority_level in args.priority_levels:
            try:
                jobs = make_different_toolset_instance(size, args.reentrant_ratio, priority_level,
                                                        toolset_slice, time_gen, rng)
            except ValueError as e:
                print(f"Skipping size={size}, priority={priority_level}: {e}")
                continue
            unique_tools = count_unique_tools(jobs, tool_unused)
            title = f"n={size},p={priority_level:.2f},r={args.reentrant_ratio},t={unique_tools}"
            write_consolidated(jobs, tool_unused, meta, title, out_dir / f"{count}.txt")
            print(f"  [{count}] {title} -> {out_dir / f'{count}.txt'}")
            count += 1

    print(f"Wrote {count} instances to {out_dir}")


# ------------------------------------------------------------------------------------------------
# CLI
# ------------------------------------------------------------------------------------------------

def int_list(s):
    return [int(x) for x in s.split(",")]


def float_list(s):
    return [float(x) for x in s.split(",")]


def add_common_args(p):
    p.add_argument("--sizes", type=int_list, default=[15, 25, 50, 75],
                    help="Comma-separated instance sizes (default: 15,25,50,75)")
    p.add_argument("--priority-levels", type=float_list, default=[0.25, 0.5, 0.75],
                    help="Comma-separated priority ratios (default: 0.25,0.5,0.75)")
    p.add_argument("--reentrant-ratio", type=float, default=0.5,
                    help="Fraction of jobs that are reentrant (default: 0.5)")
    p.add_argument("--capacity", type=int, default=DEFAULT_CAPACITY)
    p.add_argument("--machines", type=int, default=DEFAULT_MACHINES)
    p.add_argument("--days", type=int, default=DEFAULT_DAYS)
    p.add_argument("--unsupervised", type=int, default=DEFAULT_UNSUPERVISED_MINUTS)
    p.add_argument("--seed", type=int, default=None, help="Random seed (default: unseeded)")
    p.add_argument("--out-dir", type=Path, default=None,
                    help="Override the output directory (default: input/Consolidated/{SameToolSets,DiferentToolSets}). "
                         "Files are numbered 0.txt, 1.txt, ... and will overwrite any existing files with the same number.")


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest="strategy", required=True)

    same_p = sub.add_parser("same", help="Generate SameToolSets instances from real job data")
    add_common_args(same_p)
    same_p.set_defaults(func=generate_same)

    diff_p = sub.add_parser("different", help="Generate DiferentToolSets synthetic instances")
    add_common_args(diff_p)
    diff_p.set_defaults(func=generate_different)

    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
