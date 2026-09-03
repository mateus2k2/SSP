#!/usr/bin/env python3
"""
Generates SSP problem instances. Three independent strategies, one per
subcommand, each writing its own on-disk format:

  same        Consolidated single-file format (see input/Consolidated/README
              and src/loadData.cpp's SSP::loadInstanceConsolidated), written
              to input/Consolidated/SameToolSets/. A reentrant job's two
              operations SHARE one toolset -- this sharing is what
              "SameToolSets" means. Two tiers, ported from
              scripts/deprecated/myInstanceGenerator.py's makeInstaceBase +
              makeInstaceExtra: a "base" tier from real job data
              (input/Processed/{250,750,1000}.csv, after a subset-removal
              filter) and an "extra" tier of larger synthetic sizes stepped
              from input/Processed/UnusedToolSetsClean.csv. Fixed size
              ladder/reentrant ratios per tier (see same_tier_targets), not
              free CLI flags -- these are the specific profiles the tracked
              data was built from; verified to reproduce those (n, r) pairs,
              see same_tier_targets' docstring.
  different   Same Consolidated format, written to
              input/Consolidated/DiferentToolSets/. Every operation gets its
              own, never-reused toolset, drawn from
              input/Processed/UnusedToolSetsClean.csv. Targets the exact same
              (n, r) pairs as `same` (ported from the original refactory()'s
              approach of reusing an existing SameToolSets instance's n/p/r).
  base        The BaseInstances format (see src/loadData.cpp's
              SSP::loadInstanceBase), written to input/BaseInstances/
              {2M1,6M1,6M2}/. Fixed per-base-case profiles (size list,
              target reentrant ratio, target tool-ratio), matching the three
              base cases this was originally built around. Ported from
              scripts/uteis/teste.py, now scripts/deprecated/teste.py;
              verified to reproduce the tracked files exactly.

Usage:
  python3 scripts/generateInstances.py same
  python3 scripts/generateInstances.py different
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
# SAME/DIFFERENT shared target list
#
# Both same and different are driven off the SAME (n, reentrant_ratio) target pairs, ported
# from the original two-tier pipeline (scripts/deprecated/myInstanceGenerator.py's
# makeInstaceBase + makeInstaceExtra): a "base" tier built from real job data
# (input/Processed/{250,750,1000}.csv, after a subset-removal filter), and an "extra" tier of
# larger synthetic sizes stepped from input/Processed/UnusedToolSetsClean.csv. Verified against
# the currently tracked input/Consolidated/{Same,Different}ToolSets/ files: every one of the 8
# extra-tier (n, r) pairs reproduces exactly; 2 of 3 base-tier pairs reproduce exactly, the third
# is off by one job (750-filtered vs 1000-filtered convergence -- see the reconstructed
# remove_subsets() docstring for why that filter can't be pinned down bit-for-bit).
# ------------------------------------------------------------------------------------------------

SAME_BASE_TIER = [("250.csv", 0.5), ("750.csv", 0.6), ("1000.csv", 0.4)]
SAME_EXTRA_RATIO_BY_KEY = {376: 0.5, 1201: 0.6, 1401: 0.4}
SAME_EXTRA_STEP_START = 400
SAME_EXTRA_STEP_STOP = 1100   # exclusive (python range)
SAME_EXTRA_STEP = 100
SAME_EXTRA_FINAL_MAX = 1238
SAME_PRIORITY_LEVELS = [0.25, 0.5, 0.75]


def remove_subsets(tool_set_map, jobs):
    """Deterministic filter used to build the base tier's real-job pool: keep
    one job per unique ToolSet (first occurrence in source order), drop any
    whose toolset is a subset of another kept toolset, drop toolsets outside
    [1, 80] tools.

    Ported from scripts/uteis/filterJobs.py's removeSubSets -- as committed
    there, that function always returns [] (the "Remove iguais" block that
    dedupes by toolset is commented out, so it filters an empty list). This
    restores exactly what that dead block's comment specifies. Because it's
    reconstructed from a comment rather than run code, and because "one job
    per toolset" depends on Python's (deterministic, but version-dependent
    for larger structures) dict/set iteration order, this doesn't reproduce
    input/Processed/*Filtered.csv bit-for-bit -- see SAME_BASE_TIER's
    docstring for how close it actually gets."""
    seen_ids = set()
    deduped = []
    for job in jobs:
        if job['ToolSet'] not in seen_ids:
            seen_ids.add(job['ToolSet'])
            deduped.append(job)

    deduped.sort(key=lambda j: len(tool_set_map[j['ToolSet']]), reverse=True)
    kept = deduped.copy()
    removed = []
    for i, job in enumerate(deduped):
        job_tools = set(tool_set_map[job['ToolSet']])
        for other in deduped[i + 1:]:
            if other in removed:
                continue
            if set(tool_set_map[other['ToolSet']]).issubset(job_tools):
                removed.append(other)
                kept.remove(other)

    return [j for j in kept if 1 <= len(tool_set_map[j['ToolSet']]) <= 80]


def make_jobs_from_toolsets(toolset_ids, time_gen):
    """One job (single operation) per toolset id. Ported from makeJobs()."""
    times = time_gen.generate_random_numbers(len(toolset_ids))
    return [{'Job': i, 'Operation': 0, 'ToolSet': ts_id, 'Processing Time': t}
            for i, (ts_id, t) in enumerate(zip(toolset_ids, times))]


def same_tier_targets(tool_pool_len):
    """(n, reentrant_ratio, jobs_pool_factory) for every base+extra tier entry,
    where jobs_pool_factory() returns that tier's un-reentrant-assigned job pool
    (real jobs for the base tier, synthetic one-job-per-toolset for the extra
    tier) and its ToolSet lookup dict. n is the exact final operation count
    make_instance_from_pool() will produce for that pool at that ratio."""
    targets = []

    def base_factory(fname):
        def factory(tool_sets):
            jobs = loadJobs(str(PROCESSED_DIR / fname))
            return remove_subsets(tool_sets, jobs)
        return factory

    tool_sets_real = loadToolSet(str(PROCESSED_DIR / "ToolSetInt.csv"))
    for fname, ratio in SAME_BASE_TIER:
        pool = base_factory(fname)(tool_sets_real)
        ponto = int(len(pool) * ratio)
        n = len(pool) + ponto
        targets.append((n, ratio, "base", fname))

    extra_sizes = list(range(SAME_EXTRA_STEP_START, min(tool_pool_len, SAME_EXTRA_STEP_STOP), SAME_EXTRA_STEP))
    extra_sizes.append(min(tool_pool_len, SAME_EXTRA_FINAL_MAX))
    for target in extra_sizes:
        ratio_key = min(SAME_EXTRA_RATIO_BY_KEY, key=lambda k: abs(k - target))
        ratio = SAME_EXTRA_RATIO_BY_KEY[ratio_key]
        cut = int(target / (1 + ratio))
        ponto = int(cut * ratio)
        n = cut + ponto
        targets.append((n, ratio, "extra", cut))

    return targets


# ------------------------------------------------------------------------------------------------
# SAME TOOLSETS  (reentrant job's 2 operations share one toolset)
# ------------------------------------------------------------------------------------------------

def make_instance_from_pool(jobs_pool, tool_set_map, reentrant_ratio, priority_levels, time_gen, rng):
    """Port of makeInstance()'s core body: one reentrant assignment (shared
    across every priority_level, same as the original -- the unique-tool
    count doesn't depend on priority assignment, only on which jobs/toolsets
    are in the pool), producing one expanded instance per priority level.
    Returns (list of (expanded_rows, achieved_priority_ratio), unique_tools)."""
    order = list(range(len(jobs_pool)))
    rng.shuffle(order)

    ponto = int(len(jobs_pool) * reentrant_ratio)
    reentrant_positions = set(order[:ponto])
    pool = [dict(jobs_pool[idx], Reentrant=(idx in reentrant_positions)) for idx in order]
    total_ops = ponto * 2 + (len(jobs_pool) - ponto)

    unique_tools = set()
    for job in pool:
        unique_tools.update(tool_set_map[job['ToolSet']])

    results = []
    for priority_level in priority_levels:
        rng.shuffle(pool)
        priority_cutoff = int(total_ops * priority_level)
        n_priority = 0
        for job in pool:
            if n_priority < priority_cutoff:
                job['Priority'] = 1
                n_priority += 2 if job['Reentrant'] else 1
            else:
                job['Priority'] = 0

        expanded = []
        for i, job in enumerate(pool):
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

        results.append((expanded, n_priority / total_ops))

    return results, unique_tools


def generate_same(args):
    rng = random.Random(args.seed)
    time_gen = ProcessingTimeGenerator()
    tool_sets_real = loadToolSet(str(PROCESSED_DIR / "ToolSetInt.csv"))
    tool_unused = loadToolSet(str(PROCESSED_DIR / "UnusedToolSetsClean.csv"))
    tool_pool = list(tool_unused.keys())
    rng.shuffle(tool_pool)

    meta = {"CAPACITY": args.capacity, "MACHINES": args.machines,
            "DAYS": args.days, "UNSUPERVISED_MINUTS": args.unsupervised}
    out_dir = args.out_dir or (CONSOLIDATED_DIR / "SameToolSets")
    count = 0

    for fname, ratio in SAME_BASE_TIER:
        pool = remove_subsets(tool_sets_real, loadJobs(str(PROCESSED_DIR / fname)))
        results, unique_tools = make_instance_from_pool(pool, tool_sets_real, ratio, SAME_PRIORITY_LEVELS, time_gen, rng)
        for rows, achieved_p in results:
            title = f"n={len(rows)},p={achieved_p:.2f},r={ratio},t={len(unique_tools)}"
            write_consolidated(rows, tool_sets_real, meta, title, out_dir / f"{count}.txt")
            print(f"  [{count}] {title} (base:{fname}) -> {out_dir / f'{count}.txt'}")
            count += 1

    extra_sizes = list(range(SAME_EXTRA_STEP_START, min(len(tool_pool), SAME_EXTRA_STEP_STOP), SAME_EXTRA_STEP))
    extra_sizes.append(min(len(tool_pool), SAME_EXTRA_FINAL_MAX))
    for target in extra_sizes:
        ratio_key = min(SAME_EXTRA_RATIO_BY_KEY, key=lambda k: abs(k - target))
        ratio = SAME_EXTRA_RATIO_BY_KEY[ratio_key]
        cut = int(target / (1 + ratio))
        pool = make_jobs_from_toolsets(tool_pool[:cut], time_gen)
        results, unique_tools = make_instance_from_pool(pool, tool_unused, ratio, SAME_PRIORITY_LEVELS, time_gen, rng)
        for rows, achieved_p in results:
            title = f"n={len(rows)},p={achieved_p:.2f},r={ratio},t={len(unique_tools)}"
            write_consolidated(rows, tool_unused, meta, title, out_dir / f"{count}.txt")
            print(f"  [{count}] {title} (extra:{target}) -> {out_dir / f'{count}.txt'}")
            count += 1

    print(f"Wrote {count} instances to {out_dir}")


# ------------------------------------------------------------------------------------------------
# DIFFERENT TOOLSETS  (every operation gets its own, unique toolset)
# ------------------------------------------------------------------------------------------------

def make_different_toolset_instance(size, reentrant_ratio, priority_level, toolset_ids, time_gen, rng):
    """Port of makeInstanceDiferentToolSets(). Unlike make_instance_from_pool,
    `size` here is the TARGET final operation count directly (matching the
    original refactory()'s use: it read this from an existing SameToolSets
    filename rather than deriving it from a job pool), so its reentrant-count
    formula is intentionally different from make_instance_from_pool's."""
    # Matches the original's double truncation (int(N/(1+r)) then int(.../2)), not
    # int(N/(1+r)/2) -- these can differ by one at certain N/r due to where the
    # truncation happens.
    n_reentrant = int(int(size / (1 + reentrant_ratio)) / 2)
    # Each operation consumes exactly one toolset id (reentrant jobs consume 2,
    # for their 2 operations; non-reentrant consume 1), and by construction the
    # final operation count equals `size` exactly -- so `size` toolsets total,
    # not size + n_reentrant (an earlier version of this had that wrong: its
    # second loop ran range(n_reentrant, size) instead of range(2*n_reentrant,
    # size), so it built size - n_reentrant non-reentrant jobs instead of
    # size - 2*n_reentrant, overshooting both the toolset requirement and the
    # final operation count).
    needed = size
    if needed > len(toolset_ids):
        raise ValueError(f"need {needed} distinct toolsets for size={size}, only {len(toolset_ids)} available")

    jobs = []
    ts_i = 0
    for i in range(n_reentrant):
        jobs.append({'Job': i, 'Reentrant': True, 'ToolSets': [toolset_ids[ts_i], toolset_ids[ts_i + 1]]})
        ts_i += 2
    for i in range(n_reentrant * 2, size):
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

    # same (n, r) targets as generate_same, so the two folders stay comparable
    # pair-for-pair -- mirrors the original refactory() reading n/r back out of
    # each SameToolSets filename, without needing that file-scanning step.
    targets = same_tier_targets(len(tool_pool))

    for n, ratio, tier, _ in targets:
        rng.shuffle(tool_pool)
        toolset_slice = tool_pool[:n]  # exactly `n` toolsets needed; see make_different_toolset_instance
        for priority_level in SAME_PRIORITY_LEVELS:
            try:
                jobs = make_different_toolset_instance(n, ratio, priority_level, toolset_slice, time_gen, rng)
            except ValueError as e:
                print(f"Skipping n={n} ({tier}): {e}")
                continue
            unique_tools = count_unique_tools(jobs, tool_unused)
            title = f"n={n},p={priority_level:.2f},r={ratio},t={unique_tools}"
            write_consolidated(jobs, tool_unused, meta, title, out_dir / f"{count}.txt")
            print(f"  [{count}] {title} ({tier}) -> {out_dir / f'{count}.txt'}")
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


def base_assign_reentrant(jobs, setup_max):
    """Iterate jobs; decide 1 or 2 setups using a ratio-control formula:
    calc = nrWith2nd / (i+1) converges to nrWith2nd/N ~= setup_max.
    Deliberately does NOT shuffle: iterates jobs in the source CSV's fixed
    order, same as the original this was ported from. base_subset_by_ops
    later takes instances from the front of this list, so shuffling here
    would change which real jobs/toolsets land in each size -- and did, in
    an earlier version of this port, until a diff against fresh output from
    the original script caught it (see git history)."""
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
        jobs_with_reentrant = base_assign_reentrant(jobs, setup_max)

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

def add_common_args(p):
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
