import pandas as pd
import os
import json
import csv
import loadData as ld

def stringToInt(filemameIn, filemameOut,jsonName):
    
    df = pd.read_csv(filemameIn, sep=';', header=None)

    df = df.fillna('NaNPlaceholder')

    columnIndex = df.iloc[:, 0]

    df = df.drop(0, axis=1)

    uniqueStrings = df.stack().unique()
    stringToIntMapping = {string: i+1 for i, string in enumerate(uniqueStrings)}
    stringToIntMapping['NaNPlaceholder'] = ''

    df = df.applymap(lambda x: stringToIntMapping[x])
    
    df.insert(0, '', columnIndex)

    df.to_csv(filemameOut, index=False, header=False, sep=';')
    
    with open(jsonName, 'w') as fp:
        json.dump(stringToIntMapping, fp, indent=4)

def stringToIntAll(filemameIn, filemameOut, jsonName):
    df = pd.read_csv(filemameIn, sep=';', header=None)
    
    df = df.fillna('NaNPlaceholder')
    df = df.drop(0, axis=1)

    df.index = range(1, len(df) + 1)
    
    uniqueStrings = df.stack().unique()
    stringToIntMapping = {string: int(i + 1) for i, string in enumerate(uniqueStrings)}
    stringToIntMapping['NaNPlaceholder'] = ''

    df = df.applymap(lambda x: stringToIntMapping[x])
    
    df.to_csv(filemameOut, index=True, header=False, sep=';')
    
    with open(jsonName, 'w') as fp:
        json.dump(stringToIntMapping, fp, indent=4)

def pruneNotNeadedAll(filemameIn, filemameOut, typeInput):
    JobsAll1 = pd.read_csv("/home/mateus/WSL/IC/SSP/input/250.csv", delimiter=';', dtype=int)
    JobsAll2 = pd.read_csv("/home/mateus/WSL/IC/SSP/input/750.csv", delimiter=';', dtype=int)
    JobsAll3 = pd.read_csv("/home/mateus/WSL/IC/SSP/input/1000.csv", delimiter=';', dtype=int)

    toolSetNeaded1 = []
    toolSetNeaded2 = []
    toolSetNeaded3 = []

    for i in range(len(JobsAll1)):
        toolSetNeaded1.append(JobsAll1['ToolSet'][i])
    for i in range(len(JobsAll2)):
        toolSetNeaded2.append(JobsAll2['ToolSet'][i])
    for i in range(len(JobsAll3)):
        toolSetNeaded3.append(JobsAll3['ToolSet'][i])
    
    toolSetNeaded = set(toolSetNeaded1 + toolSetNeaded2 + toolSetNeaded3)

    df = pd.read_csv(filemameIn, sep=';', header=None)
    df_filtered = df[df[0].isin(toolSetNeaded)]
    
    if typeInput == int: df_filtered = df_filtered.map(lambda x: '' if pd.isna(x) else int(x))
    else: df_filtered = df_filtered.map(lambda x: '' if pd.isna(x) else (x))
    
    df_filtered.to_csv(filemameOut, index=False, sep=';', header=False)

def pruneNotNeadedOne(filemameIn, filemameOut):
    JobsAll = pd.read_csv(filemameIn, delimiter=';', dtype=int)

    toolSetNeaded = []

    for i in range(len(JobsAll)):
        toolSetNeaded.append(JobsAll['ToolSet'][i])
    
    toolSetNeaded = set(toolSetNeaded)

    df = pd.read_csv('/home/mateus/WSL/IC/SSP/input/ToolSetOG.csv', sep=';', header=None)
    df_filtered = df[df[0].isin(toolSetNeaded)]
    df_filtered = df_filtered.applymap(lambda x: '' if pd.isna(x) else (x))
    df_filtered.to_csv(filemameOut, index=False, sep=';', header=False)

# ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

def isSubSetOf(i, toolSets):
    for j in range(i, len(toolSets)):
        if ((toolSets[i] == toolSets[j]) or (set(toolSets[j]).issubset(set(toolSets[i])))) and (i != j) and (len(toolSets[j]) > 0):
            return True
    return False

def cleanUnsuedToolSets(jobs, toolSetsDict):
    indices = list(toolSetsDict.keys())
    toolSets = list(toolSetsDict.values())

    indices.sort(key=lambda x: len(toolSetsDict[x]), reverse=True)
    toolSets.sort(key=lambda x: len(x), reverse=True)
    
    toolSetsUnused = []
    incdicesUnused = []

    for i in range(len(toolSets)):
        print(f"{i+1}/{len(toolSets)}")
        
        isSubSetOfBool = isSubSetOf(i, toolSets)
        sizeBool = (len(toolSets[i]) > 0 and len(toolSets[i]) <= 80)

        if (not isSubSetOfBool) and (sizeBool):
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
        with open(f'/home/mateus/WSL/IC/SSP/input/UnusedToolSetsClean.csv', 'a', newline='') as file:
            writer = csv.writer(file, delimiter=';')
            targetLength = len(largestUnused) - (len(item) + 1)
            emptyList = [''] * targetLength
            writer.writerow([incdicesUnused[i]] + item + emptyList)

    return toolSetsUnused

def getUnsuedToolSets(jobs, toolSetsDict):
    indices = list(toolSetsDict.keys())
    toolSets = list(toolSetsDict.values())

    indices.sort(key=lambda x: len(toolSetsDict[x]), reverse=True)
    toolSets.sort(key=lambda x: len(x), reverse=True)
    
    toolSetsUnused = []
    incdicesUnused = []

    for i in range(len(toolSets)):
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
        with open(f'/home/mateus/WSL/IC/SSP/input/UnusedToolSets.csv', 'a', newline='') as file:
            writer = csv.writer(file, delimiter=';')
            targetLength = len(largestUnused) - (len(item) + 1)
            emptyList = [''] * targetLength
            writer.writerow([incdicesUnused[i]] + item + emptyList)

    return toolSetsUnused

def clenUsedToolSets(toolSetsDict):
    indices = list(toolSetsDict.keys())
    toolSets = list(toolSetsDict.values())

    indices.sort(key=lambda x: len(toolSetsDict[x]), reverse=True)
    toolSets.sort(key=lambda x: len(x), reverse=True)
    
    toolSetsUnused = []
    incdicesUnused = []

    for i in range(len(toolSets)):
        print(f"{i+1}/{len(toolSets)}")
        
        isSubSetOfBool = isSubSetOf(i, toolSets)
        sizeBool = (len(toolSets[i]) > 0 and len(toolSets[i]) <= 80)

        if (not isSubSetOfBool) and (sizeBool):
            toolSetsUnused.append(toolSets[i])
            incdicesUnused.append(indices[i])

    largestUnused = max(toolSetsUnused, key=lambda x: len(x))
    for i, item in enumerate(toolSetsUnused):    
        with open(f'/home/mateus/WSL/IC/SSP/input/UsedToolSetClean.csv', 'a', newline='') as file:
            writer = csv.writer(file, delimiter=';')
            targetLength = len(largestUnused) - (len(item) + 1)
            emptyList = [''] * targetLength
            writer.writerow([incdicesUnused[i]] + item + emptyList)

# ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

# jobs250 = ld.loadJobs("250.csv")
# jobs750 = ld.loadJobs("750.csv")
# jobs1000 = ld.loadJobs("1000.csv")
# allJobs = jobs250 + jobs750 + jobs1000

# toolSets = ld.loadToolSet("ToolSetInt.csv")
# getUnsuedToolSets(allJobs, toolSets)

# UnusedToolSets = ld.loadToolSet("UnusedToolSets.csv")
# cleanUnsuedToolSets(allJobs, UnusedToolSets)

toolSets = ld.loadToolSet("UsedToolSet.csv")
clenUsedToolSets(toolSets)

# ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

# pruneNotNeadedOne('/home/mateus/WSL/IC/SSP/input/250.csv', '/home/mateus/WSL/IC/SSP/input/250ToolSetString.csv')
# stringToInt('/home/mateus/WSL/IC/SSP/input/250ToolSetString.csv', '/home/mateus/WSL/IC/SSP/input/250ToolSet.csv', '/home/mateus/WSL/IC/SSP/input/Map/250ToolSetMapping.json')
# os.remove("/home/mateus/WSL/IC/SSP/input/250ToolSetString.csv")

# pruneNotNeadedOne('/home/mateus/WSL/IC/SSP/input/750.csv', '/home/mateus/WSL/IC/SSP/input/750ToolSetString.csv')
# stringToInt('/home/mateus/WSL/IC/SSP/input/750ToolSetString.csv', '/home/mateus/WSL/IC/SSP/input/750ToolSet.csv', '/home/mateus/WSL/IC/SSP/input/Map/750ToolSetMapping.json')
# os.remove("/home/mateus/WSL/IC/SSP/input/750ToolSetString.csv")

# pruneNotNeadedOne('/home/mateus/WSL/IC/SSP/input/1000.csv', '/home/mateus/WSL/IC/SSP/input/1000ToolSetString.csv')
# stringToInt('/home/mateus/WSL/IC/SSP/input/1000ToolSetString.csv', '/home/mateus/WSL/IC/SSP/input/1000ToolSet.csv', '/home/mateus/WSL/IC/SSP/input/Map/1000ToolSetMapping.json')
# os.remove("/home/mateus/WSL/IC/SSP/input/1000ToolSetString.csv")

# pruneNotNeadedAll('/home/mateus/WSL/IC/SSP/input/ToolSetOG.csv', '/home/mateus/WSL/IC/SSP/input/ToolSetString.csv', None)
# stringToInt('/home/mateus/WSL/IC/SSP/input/ToolSetString.csv', '/home/mateus/WSL/IC/SSP/input/UsedToolSet.csv', '/home/mateus/WSL/IC/SSP/input/Map/ToolSetMappingPruned.json')
# os.remove("/home/mateus/WSL/IC/SSP/input/ToolSetString.csv")

# stringToIntAll('/home/mateus/WSL/IC/SSP/input/ToolSetOG.csv', '/home/mateus/WSL/IC/SSP/input/ToolSetInt.csv', '/home/mateus/WSL/IC/SSP/input/Map/ToolSetMappingAll.json')
# pruneNotNeadedAll('/home/mateus/WSL/IC/SSP/input/ToolSetInt.csv', '/home/mateus/WSL/IC/SSP/input/ToolSetAll.csv', int)
# os.remove("/home/mateus/WSL/IC/SSP/input/ToolSetInt.csv")
