import pandas as pd
import matplotlib.pyplot as plt
import csv

def loadJobs(number):
    jobsFilePath = f'/home/mateus/WSL/IC/data/{number}.csv'
    jobsDF = pd.read_csv(jobsFilePath, delimiter=';')
    jobsDict = jobsDF.to_dict(orient='records')
    
    return jobsDict

def loadToolSet():
    toolSetsFilePath = '/home/mateus/WSL/IC/data/ToolSetInt.csv'
    toolSetsDF = pd.read_csv(toolSetsFilePath, delimiter=';')
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
    
    removedDuplicates.sort(key=lambda x: len(toolSet[x['ToolSet']-2]), reverse=True)
    deletedSubSets = []
    FilteredList = removedDuplicates.copy()
    
    # Remove subconjuntos
    for i, job in enumerate(removedDuplicates):
        jobSet = set(toolSet[job['ToolSet']-2])
        for j in range(i+1, len(removedDuplicates)):
            jobsInsideSet = set(toolSet[removedDuplicates[j]['ToolSet']-2])
            if jobsInsideSet.issubset(jobSet) and removedDuplicates[j] not in deletedSubSets:
                # print(f'jobSet        {jobSet}          | {job["ToolSet"]       }')
                # print(f'jobsInsideSet {jobsInsideSet}   | {removedDuplicates[j]["ToolSet"]}')
                # print("------") 
                deletedSubSets.append(removedDuplicates[j])
                FilteredList.remove(removedDuplicates[j])
    
    return FilteredList

toolSets = loadToolSet()

jobs = loadJobs(250)
saveFile(subSets(toolSets, jobs), "250Filtered.csv")

jobs = loadJobs(750)
saveFile(subSets(toolSets, jobs), "750Filtered.csv")

jobs = loadJobs(1000)
saveFile(subSets(toolSets, jobs), "1000Filtered.csv")
