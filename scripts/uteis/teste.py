import random
import os
import shutil

import loadData as ld

SIZES_2M376 = [5, 10, 15, 20, 25, 30, 50, 80, 130, 210, 376]
SIZES_6M1201 = [5, 10, 15, 20, 25, 30, 50, 80, 130, 210, 340, 630, 1201]
SIZES_6M1401 = [5, 10, 15, 20, 25, 30, 50, 80, 130, 210, 340, 630, 1401]

MACHINES = {'2M1': 2, '6M1': 6, '6M2': 6}
HORIZON = 7
UNSUPERVISED_MINUTES = 720
CAPACITY        = 80

SETUP_MAX = {'2M1': 0.5, '6M1': 0.6, '6M2': 0.4}
T_MIN     = {'2M1': 2,   '6M1': 10,  '6M2': 5}
JOB_FILES = {
    '2M1': '/workspaces/IC/SSP/input/Processed/250.csv',
    '6M1': '/workspaces/IC/SSP/input/Processed/750.csv',
    '6M2': '/workspaces/IC/SSP/input/Processed/1000.csv',
}
# Fixed tool ratio per base case: unique_tools / total_ops
# 2M1 → 1.73 (~650 unique tools from 376 ops)
# 6M1 → 1.27 (~1520 unique tools from ~1201 ops)
# 6M2 → 1.00 (~1398 unique tools from ~1398 ops)
TOOL_RATIOS    = {'2M1': 1.73, '6M1': 1.27, '6M2': 1.00}

PRIORITY_LEVEL = 0.5
SIZES          = {'2M1': SIZES_2M376, '6M1': SIZES_6M1201, '6M2': SIZES_6M1401}
TOOLSET_FILE   = '/workspaces/IC/SSP/input/Processed/ToolSetInt.csv'
OUTPUT_DIR     = '/workspaces/IC/SSP/input/BaseInstances'

def assignReentrant(jobs, setupMax):
    """
    Iterate jobs; decide 1 or 2 setups using the ratio-control formula.
    calc = nrWith2nd / (i+1)  → converges to R/N ≈ setupMax
    If calc > setupMax → 1 setup (non-reentrant), else → 2 setups (reentrant).
    """
    result = []
    nrWith2nd = 0
    for i, job in enumerate(jobs):
        calc = nrWith2nd / (i + 1)
        reentrant = calc <= setupMax
        result.append({**job, 'reentrant': reentrant})
        if reentrant:
            nrWith2nd += 1
    return result


def assignPriority(jobs, priorityLevel):
    """
    Iterate jobs; decide priority vs regular by tracking operation counts.
    calc = P_ops / (P_ops + REG_ops + 1)  → converges to priorityLevel
    If calc < priorityLevel → priority, else → regular.
    Reentrant jobs count as 2 operations.
    """
    result = []
    nrPriority = 0
    nrRegular = 0
    for job in jobs:
        calc = nrPriority / (nrPriority + nrRegular + 1)
        ops = 2 if job['reentrant'] else 1
        if calc < priorityLevel:
            result.append({**job, 'priority': 1})
            nrPriority += ops
        else:
            result.append({**job, 'priority': 0})
            nrRegular += ops
    return result


def computeUniqueTools(jobs, toolSetMap):
    unique = set()
    for job in jobs:
        unique.update(toolSetMap.get(job['ToolSet'], []))
    return unique


def finalToolRatioFix(jobs, toolSetMap, targetRatio, tMin, allToolSets, capacity=CAPACITY):
    totalOps = sum(2 if j.get('reentrant') else 1 for j in jobs)
    maxTools = max(tMin, int(totalOps * targetRatio))

    unique = computeUniqueTools(jobs, toolSetMap)
    if len(unique) <= maxTools:
        return toolSetMap

    # first maxTools unique tools encountered across jobs, in order
    unicTools = set()
    for job in jobs:
        for tool in toolSetMap.get(job['ToolSet'], []):
            unicTools.add(tool)
            if len(unicTools) >= maxTools:
                break
        if len(unicTools) >= maxTools:
            break

    # copy, then replace any tool outside unicTools with one inside (no duplicates per toolset)
    instanceTS = {k: list(v) for k, v in toolSetMap.items()}
    for job in jobs:
        tsId = job['ToolSet']
        ts = instanceTS[tsId]
        for i, tool in enumerate(ts):
            if tool not in unicTools:
                replacement = next((t for t in unicTools if t not in ts), None)
                if replacement is not None:
                    ts[i] = replacement

    return instanceTS



def adjustToolRatio(jobs, toolSetMap, targetRatio, tMin, allToolSets, capacity=CAPACITY):
    """
    Adjust per-instance toolsets so that len(uniqueTools) / totalOps ≈ targetRatio.
    (tool ratio = unique tools / total operations, where reentrant jobs count as 2 ops)

    Too low  → iterate jobs × iterate all TL refs; add novel tools that fit capacity.
    Too high → iterate jobs; if |toolset| > tMin, remove a tool unique to that TL first,
               otherwise remove the last tool and recompute.
    Returns instanceTS: a fresh per-instance copy of the relevant toolsets.
    """
    totalOps = sum(2 if j['reentrant'] else 1 for j in jobs)
    target = int(targetRatio * totalOps)

    usedTsIds = {j['ToolSet'] for j in jobs}
    instanceTS = {k: list(v) for k, v in toolSetMap.items() if k in usedTsIds}

    unique = computeUniqueTools(jobs, instanceTS)

    if len(unique) < target:
        for job in jobs:
            if len(unique) >= target:
                break
            tsId = job['ToolSet']
            for tsId2 in sorted(allToolSets.keys()):
                if len(unique) >= target:
                    break
                for tool in allToolSets[tsId2]:
                    if tool not in unique and len(instanceTS[tsId]) < capacity:
                        instanceTS[tsId].append(tool)
                        unique.add(tool)
                        break
    
    elif len(unique) > target:
        while len(unique) > target:
            prev = len(unique)
            for job in jobs:
                if len(unique) <= target:
                    break
                tsId = job['ToolSet']
                if len(instanceTS[tsId]) > tMin:
                    instanceTS[tsId].pop()
                    unique = computeUniqueTools(jobs, instanceTS)
            if len(unique) == prev:
                # print("  Warning: could not reduce tool ratio further without going below tMin", tMin)
                break

    return instanceTS


def fixOversizeToolsets(jobs, toolSetMap, capacity=CAPACITY):
    """Replace toolsets that exceed magazine capacity with a random valid one."""
    validIds = [k for k, v in toolSetMap.items() if len(v) <= capacity]
    result = []
    for job in jobs:
        if len(toolSetMap.get(job['ToolSet'], [])) > capacity:
            result.append({**job, 'ToolSet': random.choice(validIds)})
        else:
            result.append(job)
    return result


def expandJobs(jobs, sourceTimes):
    """
    Turn the job list into output rows.
    Reentrant jobs get Operation 0 and 1 (same ToolSet, second processing time
    sampled from the empirical distribution of the source data).
    """
    rows = []
    for i, job in enumerate(jobs):
        rows.append({
            'Job': i,
            'Operation': 0,
            'ToolSet': job['ToolSet'],
            'Processing Time': job['Processing Time'],
            'Priority': job['priority'],
        })
        if job['reentrant']:
            rows.append({
                'Job': i,
                'Operation': 1,
                'ToolSet': job['ToolSet'],
                'Processing Time': random.choice(sourceTimes),
                'Priority': job['priority'],
            })
    return rows


def saveInstance(rows, instanceTS, folder, name, nMachines):
    os.makedirs(folder, exist_ok=True)
    with open(f"{folder}/{name}.txt", 'w') as f:
        f.write(f"{CAPACITY} {nMachines} {HORIZON} {UNSUPERVISED_MINUTES}\n")
        f.write("\n")
        for row in rows:
            tools = ','.join(str(t) for t in row['ToolSet'])
            f.write(f"{row['Job']} {row['Operation']} {row['Processing Time']} {row['Priority']} {tools}\n")

def subsetByOps(jobs, targetOps):
    """Take jobs from the front until cumulative ops reaches targetOps."""
    result = []
    ops = 0
    for job in jobs:
        jobOps = 2 if job['reentrant'] else 1
        if ops + jobOps > targetOps:
            break
        result.append(job)
        ops += jobOps
        if ops == targetOps:
            break
    return result


def normalizeToolSets(rows, instanceTS):
    toolCount = 0
    toolMap = {}

    for i, row in enumerate(rows):
        toolSetId = row['ToolSet']
        toolSet = instanceTS[toolSetId]
        currNormalized = []
        for tool in toolSet:
            if tool not in toolMap:
                toolMap[tool] = toolCount
                toolCount += 1
            currNormalized.append(toolMap[tool])
        sortedNormalized = sorted(currNormalized)
        rows[i]['ToolSet'] = sortedNormalized
    return rows



def makeInstaceBase():
    toolSetList = ld.loadToolSet(TOOLSET_FILE)

    sourceTimes = []
    for f in JOB_FILES.values():
        for job in ld.loadJobs(f):
            sourceTimes.append(job['Processing Time'])

    for baseCase in ['2M1', '6M1', '6M2']:
        jobs = ld.loadJobs(JOB_FILES[baseCase])
        setupMax = SETUP_MAX[baseCase]
        tMin = T_MIN[baseCase]

        jobs = fixOversizeToolsets(jobs, toolSetList)
        jobsWithReentrant = assignReentrant(jobs, setupMax)

        nReentrant = sum(1 for j in jobsWithReentrant if j['reentrant'])
        print(f"\n{baseCase}: {len(jobs)} jobs → {len(jobs)+nReentrant} ops  (reentrant={nReentrant/len(jobs):.3f}, target={setupMax})")

        targetRatio = TOOL_RATIOS[baseCase]

        folder = f"{OUTPUT_DIR}/{baseCase}"
        if os.path.exists(folder):
            shutil.rmtree(folder)

        for targetSize in SIZES[baseCase]:
            subset = subsetByOps(jobsWithReentrant, targetSize)
            actualOps = sum(2 if j['reentrant'] else 1 for j in subset)

            jobsWithPriority = assignPriority(subset, PRIORITY_LEVEL)

            instanceTS = adjustToolRatio(
                jobsWithPriority, toolSetList, targetRatio, tMin, toolSetList
            )
            instanceTS = finalToolRatioFix(
                jobsWithPriority, instanceTS, targetRatio, tMin, toolSetList
            )

            uniqueTools = computeUniqueTools(jobsWithPriority, instanceTS)
            actualRatio = len(uniqueTools) / actualOps

            rows = expandJobs(jobsWithPriority, sourceTimes)
            rowsWithNormalizedToolSets = normalizeToolSets(rows, instanceTS)

            folder = f"{OUTPUT_DIR}/{baseCase}"
            name = f"n={actualOps},r={setupMax},t={len(uniqueTools)},v0"
            saveInstance(rowsWithNormalizedToolSets, instanceTS, folder, name, MACHINES[baseCase])

            print(f"  Saved {name}.txt  ratio={actualRatio:.3f} (target={targetRatio})")


makeInstaceBase()

# CAPACITY MACHINES HORIZON UNSUPERVISED_MINUTS
# JOB OPERATION PROCESSINGTIME PRIORITY TOOLSET