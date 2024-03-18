import pandas as pd
import matplotlib.pyplot as plt
import csv
import loadData as ld

# ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

def saveFile(jobs, fileName):
    fields = ["Job","Operation","ToolSet","Processing Time"]
    csv_file = open(f"/home/mateus/WSL/IC/Meu/data/{fileName}", 'w', newline='')
    csv_writer = csv.DictWriter(csv_file, fieldnames=fields, delimiter=';')
    
    csv_writer.writeheader()
    csv_writer.writerows(jobs)

# ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

def removeSubSets(toolSet, jobs):
    
    removedDuplicates = []
    
    setFound = set()
    for job in jobs:
        setFound.add(job['ToolSet'])
    
    # Remove iguais
    foundList = list(setFound)
    for item in foundList:
        for job in jobs:
            if job['ToolSet'] == item:
                removedDuplicates.append(job)
                break
    
    removedDuplicates.sort(key=lambda x: len(toolSet[x['ToolSet']]), reverse=True)
    deletedSubSets = []
    FilteredList = removedDuplicates.copy()
    
    # Remove subconjuntos
    for i, job in enumerate(removedDuplicates):
        jobSet = set(toolSet[job['ToolSet']])
        for j in range(i+1, len(removedDuplicates)):
            jobsInsideSet = set(toolSet[removedDuplicates[j]['ToolSet']])
            if jobsInsideSet.issubset(jobSet) and removedDuplicates[j] not in deletedSubSets:
                # print(f'jobSet        {jobSet}          | {job["ToolSet"]       }')
                # print(f'jobsInsideSet {jobsInsideSet}   | {removedDuplicates[j]["ToolSet"]}')
                # print("------") 
                deletedSubSets.append(removedDuplicates[j])
                FilteredList.remove(removedDuplicates[j])
    
    # Remove Larger Then 80 and Smaller Then 1
    for job in FilteredList:
        if len(toolSet[job['ToolSet']]) > 80 or len(toolSet[job['ToolSet']]) < 1:
            FilteredList.remove(job)

    return FilteredList

def getUnsuedToolSets(jobs, toolSetsDict):
    indices = list(toolSetsDict.keys())
    toolSets = list(toolSetsDict.values())

    indices.sort(key=lambda x: len(toolSetsDict[x]), reverse=True)
    toolSets.sort(key=lambda x: len(x), reverse=True)
    
    toolSetsUnused = []
    incdicesUnused = []

    for i in range(len(toolSets)):
        print(f"{i+1}/{len(toolSets)}")
        var = False
        for j in range(i, len(toolSets)):
            if ((toolSets[i] == toolSets[j]) or (set(toolSets[j]).issubset(set(toolSets[i])))) and (i != j) and (len(toolSets[j]) > 0):
                var = True
                break
        if not var and len(toolSets[i]) > 0 and len(toolSets[i]) < 80:
            toolSetsUnused.append(toolSets[i])
            incdicesUnused.append(indices[i])
    
    for job in jobs:
        if job['ToolSet'] in incdicesUnused:
            toolSetsUnused.remove(toolSetsDict[job['ToolSet']])
            incdicesUnused.remove(job['ToolSet'])

    print(f"Unused ToolSets: {len(toolSetsUnused)}")
    print(f"Total ToolSets: {len(toolSets)}")

    largestUnused = max(toolSetsUnused, key=lambda x: len(x))
    for i, item in enumerate(toolSetsUnused):    
        with open(f'/home/mateus/WSL/IC/Meu/data/UnusedToolSets.csv', 'a', newline='') as file:
            writer = csv.writer(file, delimiter=';')
            targetLength = len(largestUnused) - (len(item) + 1)
            emptyList = [''] * targetLength
            writer.writerow([incdicesUnused[i]] + item + emptyList)
                
    return toolSetsUnused

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
# saveFile(removeSubSets(toolSets, concactedJobs), "AllFiltered.csv")

# ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

# allJobs = ld.loadJobs("AllFiltered.csv")
# getUnsuedToolSets(allJobs, toolSets)

# ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
