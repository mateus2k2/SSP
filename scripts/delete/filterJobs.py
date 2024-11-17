import pandas as pd
import matplotlib.pyplot as plt
import csv
import loadData as ld

# ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

def saveFile(jobs, fileName):
    fields = ["Job","Operation","ToolSet","Processing Time"]
    csv_file = open(f"/home/mateus/WSL/IC/SSP/input/{fileName}", 'w', newline='')
    csv_writer = csv.DictWriter(csv_file, fieldnames=fields, delimiter=';')
    
    csv_writer.writeheader()
    csv_writer.writerows(jobs)

def removeSubSets(toolSet, jobs):
    
    removedDuplicates = []
    
    setFound = set()
    for job in jobs:
        setFound.add(job['ToolSet'])
    
    # Remove iguais
    # foundList = list(setFound)
    # for item in foundList:
    #     for job in jobs:
    #         if job['ToolSet'] == item:
    #             removedDuplicates.append(job)
    #             break
    
    removedDuplicates.sort(key=lambda x: len(toolSet[x['ToolSet']]), reverse=True)
    deletedSubSets = []
    FilteredList = removedDuplicates.copy()
    
    # Remove subconjuntos
    for i, job in enumerate(removedDuplicates):
        jobSet = set(toolSet[job['ToolSet']])
        for j in range(i+1, len(removedDuplicates)):
            jobsInsideSet = set(toolSet[removedDuplicates[j]['ToolSet']])
            if jobsInsideSet.issubset(jobSet) and removedDuplicates[j] not in deletedSubSets:
                deletedSubSets.append(removedDuplicates[j])
                FilteredList.remove(removedDuplicates[j])
    
    # Remove Larger Then 80 and Smaller Then 1
    for job in FilteredList:
        if len(toolSet[job['ToolSet']]) > 80 or len(toolSet[job['ToolSet']]) < 1:
            FilteredList.remove(job)

    return FilteredList

def removeSubSetsGPT(jobsOG):
    # Sort toolSets by length of toolSets[jobs['ToolSet']]
    jobsOG.sort(key=lambda x: len(toolSets[x['ToolSet']]), reverse=True)
    
    # Collect indices of jobs to remove
    to_remove = set()

    for i, jobs in enumerate(jobsOG):
        if i in to_remove:
            continue
        for j in range(i + 1, len(jobsOG)):
            if j in to_remove:
                continue
            if set(toolSets[jobsOG[j]['ToolSet']]).issubset(set(toolSets[jobs['ToolSet']])):
                to_remove.add(j)

    # remove larger then 80 and smaller then 1
    for i, job in enumerate(jobsOG):
        if len(toolSets[job['ToolSet']]) > 80 or len(toolSets[job['ToolSet']]) < 1:
            to_remove.add(i)
    
    # remove duplicates
    foundSet = set()
    for job in jobsOG:
        foundSet.add(job['ToolSet'])
    for item in foundSet:
        for i, job in enumerate(jobsOG):
            if job['ToolSet'] == item:
                to_remove.add(i)

    # Remove elements in reverse order to avoid index shifting
    for index in sorted(to_remove, reverse=True):
        del jobsOG[index]

    return jobsOG


# ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

toolSets = ld.loadToolSet("ToolSetInt.csv")

# ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

# jobs = ld.loadJobs("250.csv")
# saveFile(removeSubSets(toolSets, jobs), "250Filtered.csv")

# jobs = ld.loadJobs("750.csv")
# saveFile(removeSubSets(toolSets, jobs), "750Filtered.csv")

# jobs = ld.loadJobs("1000.csv")
# saveFile(removeSubSets(toolSets, jobs), "1000Filtered.csv")

# ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

# jobs250 = ld.loadJobs("250Filtered.csv")
# jobs750 = ld.loadJobs("750Filtered.csv")
# jobs1000 = ld.loadJobs("1000Filtered.csv")
# concactedJobs = jobs250 + jobs750 + jobs1000
# print(len(concactedJobs)) # 347
# saveFile(removeSubSets(toolSets, concactedJobs), "AllJobsFiltered.csv")


# ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

toolSets = ld.loadToolSet("UsedToolSet.csv")

jobs250 = ld.loadJobs("250.csv")
jobs750 = ld.loadJobs("750.csv")
jobs1000 = ld.loadJobs("1000.csv")

print(len(removeSubSets(toolSets, jobs250)))

