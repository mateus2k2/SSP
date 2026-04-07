import csv
import os
import reportParser as rp

def get_makespan(instance_name_path: str) -> float:
    # Extract instance name (after last '/')
    instance_name = os.path.basename(instance_name_path)

    csv_path = "./input/BeezaoRaw/alns-original.csv"

    total = 0.0
    count = 0

    try:
        with open(csv_path, newline="") as f:
            reader = csv.reader(f)

            # Skip header
            next(reader, None)

            for row in reader:
                if not row:
                    continue

                # First column: instance path/name
                field = row[0]
                clean_instance = field.split("/")[-1]

                if clean_instance != instance_name:
                    continue

                # Column index after skipping:
                # first column + 5 columns -> index 6
                makespan_value = float(row[6])
                total += makespan_value
                count += 1

    except FileNotFoundError:
        raise RuntimeError("Could not open CSV file")

    if count == 0:
        raise RuntimeError("Instance not found in CSV")

    return total / count

def get_makespan_simple(sumProcessingTimes: float, numberMachines: int) -> float:
    # round up
    return -(-sumProcessingTimes // numberMachines)


def load_instance_beezao(filename: str):
    try:
        file = open(filename, "r")
    except OSError:
        raise RuntimeError("Error opening Beezao instance file!")

    # ---------------------------------------------------------------------
    # Row 1: machines, jobs, tools, capacity
    machines, jobs, tools, capacity = map(int, file.readline().split())

    numberMachines = machines
    numberJobs = jobs
    numberJobsUngrouped = jobs
    numberToolsReal = tools
    capacityMagazine = capacity

    # ---------------------------------------------------------------------
    # Row 2: switching time
    switchingTime = int(file.readline())

    # ---------------------------------------------------------------------
    # Row 3: processing times
    processingTimes = list(map(int, file.readline().split()))
    if len(processingTimes) != jobs:
        raise RuntimeError("Invalid processing times row")

    # ---------------------------------------------------------------------
    # Remaining rows: tool × job matrix
    toolMatrix = []
    for _ in range(tools):
        row = list(map(int, file.readline().split()))
        if len(row) != jobs:
            raise RuntimeError("Invalid tool matrix row")
        toolMatrix.append(row)

    # ---------------------------------------------------------------------
    # Create ToolSets (one per job)
    originalToolSets = {}

    for j in range(jobs):
        tools_for_job = []
        for t in range(tools):
            if toolMatrix[t][j] == 1:
                tools_for_job.append(t)

        originalToolSets[j] = {
            "indexToolSet": j,
            "tools": tools_for_job
        }

    # ---------------------------------------------------------------------
    # Create Jobs
    originalJobs = []
    numberOfPriorityJobs = 0

    for j in range(jobs):
        job = {
            "indexJob": j,
            "indexOperation": 0,
            "processingTime": processingTimes[j],
            "priority": 1,
            "toolSet": originalToolSets[j],
            # "toolSetNormalized": originalToolSets[j],
        }

        numberOfPriorityJobs += job["priority"]
        originalJobs.append(job)

    # ---------------------------------------------------------------------
    # Build tool-job boolean matrix
    numberTools = tools
    toolJob = [[False for _ in range(numberJobs)] for _ in range(numberTools)]

    for j in range(jobs):
        for t in originalToolSets[j]["tools"]:
            toolJob[t][j] = True

    # ---------------------------------------------------------------------
    # Planning horizon
    # planingHorizon = get_makespan(filename)
    planingHorizon = get_makespan_simple(sum(processingTimes), numberMachines)
    unsupervised = planingHorizon

    file.close()

    return {
        "originalJobs": originalJobs,
        "numberMachines": numberMachines,
        "numberJobs": numberJobs,
        "capacityMagazine": capacityMagazine,
        "switchingTime": switchingTime,
        "planingHorizon": planingHorizon,
        "unsupervised": unsupervised,
        "numberOfPriorityJobs": numberOfPriorityJobs
    }

def validate_solution_toolsets(originalData: dict, solution: tuple):
    """
    Validates whether the magazine of each scheduled job
    contains all tools required by the job's toolset.

    Returns:
        valid (bool)
        errors (list of dicts)
    """

    # Build fast lookup: job_id -> required tools (as set)
    required_tools_by_job = {
        job["indexJob"]: set(job["toolSet"]["tools"])
        for job in originalData["originalJobs"]
    }

    _, machines, _ = solution

    errors = []

    for machine_id, machine in enumerate(machines):
        for op in machine:
            job_id = op["job"]
            magazine = set(op["magazine"])

            required = required_tools_by_job[job_id]
            missing = required - magazine

            if missing:
                errors.append({
                    "machine": machine_id,
                    "job": job_id,
                    "missing_tools": sorted(missing),
                    "required_tools": sorted(required),
                    "magazine": sorted(magazine),
                    "start": op["start"],
                    "end": op["end"]
                })

    return len(errors) == 0, errors

def validate_toolset_capacity(originalData: dict):
    """
    Validates that each job's required toolset size
    does not exceed the magazine capacity.

    Returns:
        valid (bool)
        errors (list of dicts)
    """

    capacity = originalData["capacityMagazine"]
    jobs = originalData["originalJobs"]

    errors = []

    for job in jobs:
        job_id = job["indexJob"]
        tools = job["toolSet"]["tools"]
        tool_count = len(tools)

        if tool_count > capacity:
            errors.append({
                "job": job_id,
                "tool_count": tool_count,
                "capacity": capacity,
                "tools": sorted(tools)
            })

    return len(errors) == 0, errors

def validate_all_jobs_finished(originalData: dict, solution: tuple):
    """
    Validates that all jobs are finished exactly once in the solution.

    Returns:
        valid (bool)
        report (dict)
    """

    number_jobs = originalData["numberJobs"]
    expected_jobs = set(range(number_jobs))

    _, machines, _ = solution

    finished_jobs = []

    for machine in machines:
        for op in machine:
            finished_jobs.append(op["job"])

    finished_jobs_set = set(finished_jobs)

    missing_jobs = sorted(expected_jobs - finished_jobs_set)
    duplicated_jobs = sorted(
        job for job in finished_jobs_set
        if finished_jobs.count(job) > 1
    )

    valid = not missing_jobs and not duplicated_jobs

    report = {
        "total_jobs": number_jobs,
        "finished_jobs_count": len(finished_jobs),
        "unique_finished_jobs": len(finished_jobs_set),
        "missing_jobs": missing_jobs,
        "duplicated_jobs": duplicated_jobs
    }

    return valid, report



# Instancia
# Tarefas Finalizadas
# Trocas
# Instancias de troca
# Tarefas não finalizads
# Trocas na maquina criticas
# Final Solution
# Time
# PTL
# Medias das soluções iniciais
def fmt(n):
    return str(round(n, 4)).replace(".", ",")

def processRunFolder(folderPath, accumulator):
    """Parse all .PMTC files in a single run folder and accumulate results."""
    for filename in os.listdir(folderPath):
        if not filename.endswith(".PMTC"):
            continue

        filepath = os.path.join(folderPath, filename)
        print(f"  Validating {filename}...")
        solution = rp.parseReport(filepath)
        originalData = load_instance_beezao(solution[0]["jobsFileName"])

        valid, errors = validate_solution_toolsets(originalData, solution)
        if not valid:
            print(f"    ❌ Toolset validation failed with {len(errors)} errors.")

        valid, errors = validate_toolset_capacity(originalData)
        if not valid:
            print(f"    ❌ Capacity validation failed with {len(errors)} errors.")

        valid, report = validate_all_jobs_finished(originalData, solution)
        if not valid:
            print(f"    ❌ Job completion validation failed.")
            print(report)

        planejamentoObj, machines, endInfoObj = solution
        instance_name = planejamentoObj["jobsFileName"].split("/")[-1].replace(".PMTC", "")
        final = (30 * endInfoObj["fineshedJobsCount"]
                 - (endInfoObj["switchs"]
                    + 10 * endInfoObj["switchsInstances"]
                    + 30 * endInfoObj["totalUnfineshed"]))

        entry = accumulator.setdefault(instance_name, {
            "Instancia": instance_name,
            "Tarefas Finalizadas": [],
            "Tarefas não finalizads": [],
            "Trocas": [],
            "Instancias de troca": [],
            "Soluções Final": [],
            "Soluções Final Real": [],
            "Tempo": [],
            "Soluções Iniciais": [],
            "PTL": [],
        })

        entry["Tarefas Finalizadas"].append(endInfoObj["fineshedJobsCount"])
        entry["Tarefas não finalizads"].append(endInfoObj["totalUnfineshed"])
        entry["Trocas"].append(endInfoObj["switchs"])
        entry["Instancias de troca"].append(endInfoObj["switchsInstances"])
        entry["Soluções Final"].append(endInfoObj["finalSolution"])
        entry["Soluções Final Real"].append(final)
        entry["Tempo"].append(endInfoObj["Time"] / 1000)
        if "bestInitial" in endInfoObj:
            entry["Soluções Iniciais"].append(endInfoObj["bestInitial"])
        if "PTL" in endInfoObj:
            entry["PTL"].append(endInfoObj["PTL"])


def validateFolder(topFolderPath):
    """
    topFolderPath should contain numbered subfolders (one per execution run).
    Results for the same instance are averaged across runs and written to CSV.
    """
    accumulator = {}

    run_dirs = sorted(
        d for d in os.listdir(topFolderPath)
        if os.path.isdir(os.path.join(topFolderPath, d))
    )

    for run_dir in run_dirs:
        run_path = os.path.join(topFolderPath, run_dir)
        print(f"Processing run: {run_dir}")
        processRunFolder(run_path, accumulator)

    # Build averaged rows
    csvData = []
    for instance_name, entry in accumulator.items():
        def mean(lst):
            return sum(lst) / len(lst) if lst else 0

        csvData.append({
            "Instancia": instance_name,
            "Tarefas Finalizadas": fmt(mean(entry["Tarefas Finalizadas"])),
            "Tarefas não finalizads": fmt(mean(entry["Tarefas não finalizads"])),
            "Trocas": fmt(mean(entry["Trocas"])),
            "Instancias de troca": fmt(mean(entry["Instancias de troca"])),
            "Soluções Iniciais": fmt(mean(entry["Soluções Iniciais"])),
            "Soluções Final": fmt(mean(entry["Soluções Final"])),
            "Soluções Final Real": fmt(mean(entry["Soluções Final Real"])),
            "Tempo": fmt(mean(entry["Tempo"])),
            "PTL": fmt(mean(entry["PTL"])),
        })

    csvData.sort(key=lambda x: int(x["Instancia"].split("_")[0].replace("instanceLarge", "").replace("instance", "")))

    output_csv = os.path.join(topFolderPath, "results_mean.csv")
    fieldnames = [
        "Instancia",
        "Tarefas Finalizadas",
        "Tarefas não finalizads",
        "Trocas",
        "Instancias de troca",
        "Soluções Iniciais",
        "Soluções Final",
        "Soluções Final Real",
        "Tempo",
        "PTL",
    ]
    with open(output_csv, "w", newline="") as csvfile:
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames, delimiter=";")
        writer.writeheader()
        for row in csvData:
            writer.writerow(row)

    print(f"\nDone. {len(csvData)} instances averaged across {len(run_dirs)} runs.")
    print(f"Output: {output_csv}")


validateFolder("/workspaces/IC/SSP/output/Beezao")