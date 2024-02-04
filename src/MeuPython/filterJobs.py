import pandas as pd
import matplotlib.pyplot as plt
import csv

# ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

def loadJobs(number):
    jobsFilePath = f'/home/mateus/WSL/IC/data/{number}.csv'
    jobsDF = pd.read_csv(jobsFilePath, delimiter=';')
    jobsDict = jobsDF.to_dict(orient='records')
    
    return jobsDict

def loadToolSet():
    toolSetsFilePath = '/home/mateus/WSL/IC/data/ToolSetInt.csv'
    toolSetsDF = pd.read_csv(toolSetsFilePath, header=None, delimiter=';')
    
    indices = toolSetsDF.columns[0]
    indicesList = [indices]
    
    toolSetsDF = toolSetsDF.drop(toolSetsDF.columns[0], axis=1)
    toolSetsDF = toolSetsDF.fillna('NaNPlaceholder')
    toolSetsDict = toolSetsDF.to_dict(orient='records')
    toolSetList = []
    for i in range(0, len(toolSetsDict)):
        toolSetList.append([])
        for coisa in toolSetsDict[i].values():
            if coisa != 'NaNPlaceholder':
                toolSetList[i].append(int(coisa))

    return toolSetList

def saveFile(jobs, fileName):
    fields = ["Job","Operation","ToolSet","Processing Time"]
    csv_file = open(f"/home/mateus/WSL/IC/data/{fileName}", 'w', newline='')
    csv_writer = csv.DictWriter(csv_file, fieldnames=fields, delimiter=';')
    
    csv_writer.writeheader()
    csv_writer.writerows(jobs)

# ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

def subSets(toolSet, jobs):
    
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
    
    removedDuplicates.sort(key=lambda x: len(toolSet[x['ToolSet']-1]), reverse=True)
    deletedSubSets = []
    FilteredList = removedDuplicates.copy()
    
    # Remove subconjuntos
    for i, job in enumerate(removedDuplicates):
        jobSet = set(toolSet[job['ToolSet']-1])
        for j in range(i+1, len(removedDuplicates)):
            jobsInsideSet = set(toolSet[removedDuplicates[j]['ToolSet']-1])
            if jobsInsideSet.issubset(jobSet) and removedDuplicates[j] not in deletedSubSets:
                # print(f'jobSet        {jobSet}          | {job["ToolSet"]       }')
                # print(f'jobsInsideSet {jobsInsideSet}   | {removedDuplicates[j]["ToolSet"]}')
                # print("------") 
                deletedSubSets.append(removedDuplicates[j])
                FilteredList.remove(removedDuplicates[j])
    
    return FilteredList


def getUnsuedToolSets(jobs, toolSets):
    indices = [i for i in range(1, len(toolSets)+1)]
    indices.sort(key=lambda x: len(toolSets[x-1]), reverse=True)
    toolSets.sort(key=lambda x: len(x), reverse=True)
    
    toolSetsUnused = []
    incdicesUnused = []

    for i, toolSet in enumerate(toolSets):
        print(f"{i+1}/{len(toolSets)}")
        for job in jobs:
            if not((toolSets[job["ToolSet"]-1] == toolSet) or (toolSets[job["ToolSet"]-1] in toolSet) or (toolSet in toolSets[job["ToolSet"]-1])) and (toolSet not in toolSetsUnused):
                toolSetsUnused.append(toolSet)
                incdicesUnused.append(indices[i])
                # print(f"{indices[i]} = {toolSet}")
                
    largestUnused = max(toolSetsUnused, key=lambda x: len(x))
    for i, item in enumerate(toolSetsUnused):    
        with open(f'/home/mateus/WSL/IC/data/UnusedToolSets.csv', 'a', newline='') as file:
            writer = csv.writer(file, delimiter=';')
            targetLength = len(largestUnused) - (len(item) + 1)
            emptyList = [''] * targetLength
            writer.writerow([incdicesUnused[i]] + item + emptyList)
                
    return toolSetsUnused

# ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

toolSets = loadToolSet()

# ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

# jobs = loadJobs(250)
# saveFile(subSets(toolSets, jobs), "250Filtered.csv")

# jobs = loadJobs(750)
# saveFile(subSets(toolSets, jobs), "750Filtered.csv")

# jobs = loadJobs(1000)
# saveFile(subSets(toolSets, jobs), "1000Filtered.csv")

# ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

# jobs250 = loadJobs("250Filtered")
# jobs750 = loadJobs("750Filtered")
# jobs1000 = loadJobs("1000Filtered")
# concactedJobs = jobs250 + jobs750 + jobs1000
# print(len(concactedJobs)) # 347
# saveFile(subSets(toolSets, concactedJobs), "AllFiltered.csv")


# ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

allJobs = loadJobs("AllFiltered")
getUnsuedToolSets(allJobs, toolSets)