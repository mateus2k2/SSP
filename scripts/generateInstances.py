#!/usr/bin/env python3
"""
Generates SSP problem instances. Three independent strategies, one per
subcommand, each writing its own on-disk format:

  same        Consolidated single-file format (see input/Consolidated/README
              and src/loadData.cpp's SSP::loadInstanceConsolidated), written
              to input/Consolidated/SameToolSets/. Built from real job data
              (input/Processed/{250,750,1000}.csv); a reentrant job's two
              operations SHARE one toolset drawn from the real pool
              (input/Processed/ToolSetInt.csv) -- this sharing is what
              "SameToolSets" means.
  different   Same Consolidated format, written to
              input/Consolidated/DiferentToolSets/. Synthetic instances
              where every operation gets its own, never-reused toolset,
              drawn from input/Processed/UnusedToolSetsClean.csv.
  base        The BaseInstances format (see src/loadData.cpp's
              SSP::loadInstanceBase), written to input/BaseInstances/
              {2M1,6M1,6M2}/. Fixed per-base-case profiles (size list,
              target reentrant ratio, target tool-ratio) rather than
              free-form --sizes/--priority-levels, matching the three
              base cases this was originally built around. Ported from
              scripts/uteis/teste.py, now scripts/deprecated/teste.py.

same/different vary instance size, priority level, and reentrant ratio via
CLI flags. Does NOT touch BeezaoRaw/ -- that's a separate, external
benchmark dataset with published reference results, not something this
generates.

Usage:
  python3 scripts/generateInstances.py same --sizes 15,25,50,75 --priority-levels 0.25,0.5,0.75
  python3 scripts/generateInstances.py different --sizes 15,25,50,75
  python3 scripts/generateInstances.py base
  python3 scripts/generateInstances.py base --cases 2M1,6M1

All paths are relative to the repo root (resolved from this file's
location), so this runs the same regardless of which machine/checkout it's
run from.
"""
import argparse
import random
import shutil
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
# BASE INSTANCES  (BaseInstances format, three fixed research profiles: 2M1/6M1/6M2)
#
# Ported from scripts/uteis/teste.py (now scripts/deprecated/teste.py). Unlike same/different,
# this isn't a free-size generator: each base case has a fixed size ladder, target reentrant
# ratio, and target tool-ratio (unique tools / total operations) that the source data was
# originally profiled against, so those stay as named constants rather than CLI flags.
# ------------------------------------------------------------------------------------------------

BASE_SIZES = {
    '2M1': [5, 10, 15, 20, 25, 30, 50, 80, 130, 210, 376],
    '6M1': [5, 10, 15, 20, 25, 30, 50, 80, 130, 210, 340, 630, 1201],
    '6M2': [5, 10, 15, 20, 25, 30, 50, 80, 130, 210, 340, 630, 1401],
}
BASE_MACHINES = {'2M1': 2, '6M1': 6, '6M2': 6}
BASE_SETUP_MAX = {'2M1': 0.5, '6M1': 0.6, '6M2': 0.4}   # target reentrant ratio
BASE_T_MIN = {'2M1': 2, '6M1': 10, '6M2': 5}            # minimum toolset size kept per job
BASE_JOB_FILES = {'2M1': "250.csv", '6M1': "750.csv", '6M2': "1000.csv"}
# Fixed tool ratio per base case: unique_tools / total_ops
#   2M1 -> 1.73 (~650 unique tools from 376 ops)
#   6M1 -> 1.27 (~1520 unique tools from ~1201 ops)
#   6M2 -> 1.00 (~1398 unique tools from ~1398 ops)
BASE_TOOL_RATIOS = {'2M1': 1.73, '6M1': 1.27, '6M2': 1.00}
BASE_PRIORITY_LEVEL = 0.5
BASE_CAPACITY = DEFAULT_CAPACITY
BASE_HORIZON = 7
BASE_UNSUPERVISED_MINUTS = 720


def base_assign_reentrant(jobs, setup_max, rng):
    """Iterate jobs; decide 1 or 2 setups using a ratio-control formula:
    calc = nrWith2nd / (i+1) converges to nrWith2nd/N ~= setup_max."""
    jobs = jobs[:]
    rng.shuffle(jobs)
    result = []
    n_with_2nd = 0
    for i, job in enumerate(jobs):
        calc = n_with_2nd / (i + 1)
        reentrant = calc <= setup_max
        result.append({**job, 'reentrant': reentrant})
        if reentrant:
            n_with_2nd += 1
    return result


def base_assign_priority(jobs, priority_level):
    """Same ratio-control approach as base_assign_reentrant, tracking operation
    counts (reentrant jobs count as 2 ops) instead of job counts."""
    result = []
    n_priority = 0
    n_regular = 0
    for job in jobs:
        calc = n_priority / (n_priority + n_regular + 1)
        ops = 2 if job['reentrant'] else 1
        if calc < priority_level:
            result.append({**job, 'priority': 1})
            n_priority += ops
        else:
            result.append({**job, 'priority': 0})
            n_regular += ops
    return result


def base_unique_tools(jobs, tool_set_map):
    unique = set()
    for job in jobs:
        unique.update(tool_set_map.get(job['ToolSet'], []))
    return unique


def base_fix_oversize_toolsets(jobs, tool_set_map, capacity, rng):
    """Replace toolsets that exceed magazine capacity with a random valid one."""
    valid_ids = [k for k, v in tool_set_map.items() if len(v) <= capacity]
    result = []
    for job in jobs:
        if len(tool_set_map.get(job['ToolSet'], [])) > capacity:
            result.append({**job, 'ToolSet': rng.choice(valid_ids)})
        else:
            result.append(job)
    return result


def base_adjust_tool_ratio(jobs, tool_set_map, target_ratio, t_min, all_tool_sets, capacity):
    """Adjust per-instance toolsets so unique_tools/total_ops ~= target_ratio.
    Too low: add novel tools that fit capacity. Too high: shrink toolsets down
    to t_min. Returns a fresh per-instance copy of the relevant toolsets."""
    total_ops = sum(2 if j['reentrant'] else 1 for j in jobs)
    target = int(target_ratio * total_ops)

    used_ts_ids = {j['ToolSet'] for j in jobs}
    instance_ts = {k: list(v) for k, v in tool_set_map.items() if k in used_ts_ids}

    unique = base_unique_tools(jobs, instance_ts)

    if len(unique) < target:
        for job in jobs:
            if len(unique) >= target:
                break
            ts_id = job['ToolSet']
            for ts_id2 in sorted(all_tool_sets.keys()):
                if len(unique) >= target:
                    break
                for tool in all_tool_sets[ts_id2]:
                    if tool not in unique and len(instance_ts[ts_id]) < capacity:
                        instance_ts[ts_id].append(tool)
                        unique.add(tool)
                        break
    elif len(unique) > target:
        while len(unique) > target:
            prev = len(unique)
            for job in jobs:
                if len(unique) <= target:
                    break
                ts_id = job['ToolSet']
                if len(instance_ts[ts_id]) > t_min:
                    instance_ts[ts_id].pop()
                    unique = base_unique_tools(jobs, instance_ts)
            if len(unique) == prev:
                break  # can't reduce further without going below t_min

    return instance_ts


def base_final_tool_ratio_fix(jobs, tool_set_map, target_ratio, t_min):
    """Hard cap: if base_adjust_tool_ratio still overshot (e.g. capacity-limited),
    force every toolset down to only the first `maxTools` unique tools seen."""
    total_ops = sum(2 if j.get('reentrant') else 1 for j in jobs)
    max_tools = max(t_min, int(total_ops * target_ratio))

    unique = base_unique_tools(jobs, tool_set_map)
    if len(unique) <= max_tools:
        return tool_set_map

    kept_tools = set()
    for job in jobs:
        for tool in tool_set_map.get(job['ToolSet'], []):
            kept_tools.add(tool)
            if len(kept_tools) >= max_tools:
                break
        if len(kept_tools) >= max_tools:
            break

    instance_ts = {k: list(v) for k, v in tool_set_map.items()}
    for job in jobs:
        ts = instance_ts[job['ToolSet']]
        for i, tool in enumerate(ts):
            if tool not in kept_tools:
                replacement = next((t for t in kept_tools if t not in ts), None)
                if replacement is not None:
                    ts[i] = replacement

    return instance_ts


def base_subset_by_ops(jobs, target_ops):
    """Take jobs from the front until cumulative operation count reaches target_ops."""
    result = []
    ops = 0
    for job in jobs:
        job_ops = 2 if job['reentrant'] else 1
        if ops + job_ops > target_ops:
            break
        result.append(job)
        ops += job_ops
        if ops == target_ops:
            break
    return result


def base_expand_jobs(jobs, source_times, rng):
    """Reentrant jobs get Operation 0 and 1 (same ToolSet, second processing
    time sampled from the empirical distribution of the source data)."""
    rows = []
    for i, job in enumerate(jobs):
        rows.append({'Job': i, 'Operation': 0, 'ToolSet': job['ToolSet'],
                      'Processing Time': job['Processing Time'], 'Priority': job['priority']})
        if job['reentrant']:
            rows.append({'Job': i, 'Operation': 1, 'ToolSet': job['ToolSet'],
                          'Processing Time': rng.choice(source_times), 'Priority': job['priority']})
    return rows


def base_normalize_toolsets(rows, instance_ts):
    tool_map = {}
    for row in rows:
        dense = []
        for tool in instance_ts[row['ToolSet']]:
            if tool not in tool_map:
                tool_map[tool] = len(tool_map)
            dense.append(tool_map[tool])
        row['ToolSet'] = sorted(dense)
    return rows


def base_save_instance(rows, out_path, n_machines):
    out_path.parent.mkdir(parents=True, exist_ok=True)
    with open(out_path, "w") as f:
        f.write(f"{BASE_CAPACITY} {n_machines} {BASE_HORIZON} {BASE_UNSUPERVISED_MINUTS}\n\n")
        for row in rows:
            tools = ",".join(str(t) for t in row['ToolSet'])
            f.write(f"{row['Job']} {row['Operation']} {row['Processing Time']} {row['Priority']} {tools}\n")


def generate_base(args):
    rng = random.Random(args.seed)
    tool_set_list = loadToolSet(str(PROCESSED_DIR / "ToolSetInt.csv"))

    source_times = []
    for fname in BASE_JOB_FILES.values():
        for job in loadJobs(str(PROCESSED_DIR / fname)):
            source_times.append(job['Processing Time'])

    out_root = args.out_dir or (INPUT_DIR / "BaseInstances")

    for case in args.cases:
        jobs = loadJobs(str(PROCESSED_DIR / BASE_JOB_FILES[case]))
        setup_max = BASE_SETUP_MAX[case]
        t_min = BASE_T_MIN[case]
        target_ratio = BASE_TOOL_RATIOS[case]

        jobs = base_fix_oversize_toolsets(jobs, tool_set_list, BASE_CAPACITY, rng)
        jobs_with_reentrant = base_assign_reentrant(jobs, setup_max, rng)

        n_reentrant = sum(1 for j in jobs_with_reentrant if j['reentrant'])
        print(f"{case}: {len(jobs)} jobs -> {len(jobs) + n_reentrant} ops "
              f"(reentrant={n_reentrant / len(jobs):.3f}, target={setup_max})")

        folder = out_root / case
        if args.clean and folder.exists():
            shutil.rmtree(folder)

        for target_size in BASE_SIZES[case]:
            subset = base_subset_by_ops(jobs_with_reentrant, target_size)
            actual_ops = sum(2 if j['reentrant'] else 1 for j in subset)

            jobs_with_priority = base_assign_priority(subset, BASE_PRIORITY_LEVEL)

            instance_ts = base_adjust_tool_ratio(jobs_with_priority, tool_set_list, target_ratio,
                                                  t_min, tool_set_list, BASE_CAPACITY)
            instance_ts = base_final_tool_ratio_fix(jobs_with_priority, instance_ts, target_ratio, t_min)

            unique_tools = base_unique_tools(jobs_with_priority, instance_ts)
            actual_ratio = len(unique_tools) / actual_ops

            rows = base_expand_jobs(jobs_with_priority, source_times, rng)
            rows = base_normalize_toolsets(rows, instance_ts)

            name = f"n={actual_ops},r={setup_max},t={len(unique_tools)},v0.txt"
            base_save_instance(rows, folder / name, BASE_MACHINES[case])
            print(f"  Saved {case}/{name}  ratio={actual_ratio:.3f} (target={target_ratio})")


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

    base_p = sub.add_parser("base", help="Generate the BaseInstances 2M1/6M1/6M2 profiles")
    base_p.add_argument("--cases", type=lambda s: s.split(","), default=["2M1", "6M1", "6M2"],
                         help="Comma-separated base cases to (re)generate (default: 2M1,6M1,6M2)")
    base_p.add_argument("--seed", type=int, default=None, help="Random seed (default: unseeded)")
    base_p.add_argument("--out-dir", type=Path, default=None,
                         help="Override the output directory (default: input/BaseInstances). "
                              "Each case writes into <out-dir>/<case>/.")
    base_p.add_argument("--clean", action="store_true",
                         help="Delete <out-dir>/<case>/ before writing (default: off -- files are "
                              "overwritten by name, but stale files from a previous run with a "
                              "different --cases/size profile are left in place).")
    base_p.set_defaults(func=generate_base)

    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
