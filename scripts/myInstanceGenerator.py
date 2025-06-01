import pandas as pd
import random
import csv
import os

import uteis.loadData as ld
from uteis.ProcessingTimeGenerator import ProcessingTimeGenerator

geradorProcessingTime = ProcessingTimeGenerator()

# ------------------------------------------------------------------------------------------------
# REFACTORY
# ------------------------------------------------------------------------------------------------

def makeSmallDiff():
    baseInstance = "/home/mateus/WSL/IC/SSP/input/MyInstancesDiferentToolSets/n=75,p=0.24,r=0.5,t=112,v0.csv"
    outputFolder = "/home/mateus/WSL/IC/SSP/input/MyInstancesDiferentToolSets/new"
    baseFolder = "/home/mateus/WSL/IC/SSP/input/MyInstancesDiferentToolSets"
    # take the 9 first .csv files in the folder
    files = os.listdir(baseFolder)
    files = [f"{baseFolder}/{file}" for file in files if file.endswith('.csv')]
    files = sorted(files, key=lambda x: int(x.split('=')[1].split(',')[0]))
    files = files[:9]

    baseInstance = pd.read_csv(baseInstance, delimiter=';')
    baseInstance = baseInstance.to_dict(orient='records')
    for file in files:
        print(f"Processing file: {file}")
        curInstance = pd.read_csv(file, delimiter=';')
        curInstance = curInstance.to_dict(orient='records')
        fileName = file.split('/')[-1]
        newInstancesCSV = open(f"{outputFolder}/{fileName}", 'w+', newline='')
        fields = ["Job","Operation","ToolSet","Processing Time","Priority"]
        newInstances = csv.DictWriter(newInstancesCSV, fieldnames=fields, delimiter=';')
        newInstances.writeheader()
        for index, item in enumerate(curInstance):
            newInstances.writerow({
                "Job": item['Job'],
                "Operation": item['Operation'],
                "Processing Time": item['Processing Time'],
                "ToolSet": baseInstance[index]['ToolSet'],
                "Priority": item['Priority'],
            })

def creackTools():
    baseInstance = '/home/mateus/WSL/IC/SSP/input/MyInstancesSameToolSets/n=25,p=0.5,r=0.5,t=0,v0.csv'
    baseInstance = pd.read_csv(baseInstance, delimiter=';')
    baseInstance = baseInstance.to_dict(orient='records')

    toolSetList = ld.loadToolSet("/home/mateus/WSL/IC/SSP/input/Processed/ToolSetInt.csv")
    tools = set()
    for item in baseInstance:
        curTools = toolSetList[item['ToolSet']]
        tools = tools.union(set(curTools))

    print(f"Total de ferramentas: {len(tools)}")

def createSmallerInstances():
    baseInstance = '/home/mateus/WSL/IC/SSP/input/MyInstancesSameToolSets/n=75,p=0.24,r=0.5,t=650,v0.csv'
    baseInstance = pd.read_csv(baseInstance, delimiter=';')
    baseInstance = baseInstance.to_dict(orient='records')

    baseInstanceUnicJobs = []
    for item in baseInstance:
        if item['Operation'] == 0:
            baseInstanceUnicJobs.append(item)


    # print(len(baseInstanceUnicJobs))
    # shuffle the list
    newInstancesSizes = [25]
    reentrantRatio = 0.5

    priorityLevels = [0.25, 0.5, 0.75]
    for priorityLevel in priorityLevels:
        instanciesToCreate = []
        random.shuffle(baseInstanceUnicJobs)
        for size in newInstancesSizes:
            toMakeReentrant = int(size/(1+reentrantRatio))
            toMakeReentrant = int(toMakeReentrant/2)
            if size == 15: toMakeReentrant = 2
            for i in range(0, toMakeReentrant):
                instanciesToCreate.append({
                    "Job": i,
                    "Operation": 0,
                    "Processing Time": baseInstanceUnicJobs[i]['Processing Time'],
                    "ToolSet": baseInstanceUnicJobs[i]['ToolSet'],
                    "Priority": 0,
                    "Reentrant": True,
                })
            for i in range(toMakeReentrant*2, size):
                instanciesToCreate.append({
                    "Job": i,
                    "Operation": 0,
                    "Processing Time": baseInstanceUnicJobs[i]['Processing Time'],
                    "ToolSet": baseInstanceUnicJobs[i]['ToolSet'],
                    "Priority": 0,
                    "Reentrant": False,
                })
            # shuffle the list
            random.shuffle(instanciesToCreate)
            numberOfPriority = int(size * priorityLevel)
            for i in range(0, len(instanciesToCreate)):
                if numberOfPriority <= 0: break
                if instanciesToCreate[i]['Reentrant']:
                    instanciesToCreate[i]['Priority'] = 1
                    numberOfPriority -= 2
                else:
                    instanciesToCreate[i]['Priority'] = 1
                    numberOfPriority -= 1
            folderToSave = f"/home/mateus/WSL/IC/SSP/input/MyInstancesSameToolSets/Test"
            fileName = f"n={size},p={priorityLevel},r=0.5,t=0,v0.csv"
            csv_file = open(f"{folderToSave}/{fileName}", 'w+', newline='')
            fields = ["Job","Operation","ToolSet","Processing Time","Priority"]
            csv_writer = csv.DictWriter(csv_file, fieldnames=fields, delimiter=';')
            csv_writer.writeheader()
            for i, item in enumerate(instanciesToCreate):
                csv_writer.writerow({
                    "Job": i,
                    "Operation": 0,
                    "ToolSet": item['ToolSet'],
                    "Processing Time": item['Processing Time'],
                    "Priority": item['Priority'],
                })
                if item['Reentrant'] == True:
                    csv_writer.writerow({
                        "Job": i,
                        "Operation": 1,
                        "ToolSet": item['ToolSet'],
                        "Processing Time": item['Processing Time'],
                        "Priority": item['Priority'],
                    })

            csv_file.close()
            # create a .dat file with the same name 

            dat_file = open(f"{folderToSave}/{fileName.replace('.csv', '.dat')}", 'w')
            dat_file.write("CAPACITY 80\n")
            dat_file.write("MACHINES 2\n")
            dat_file.write("DAYS 2\n")
            dat_file.write("UNSUPERVISED_MINUTS 720\n")

# ------------------------------------------------------------------------------------------------
# REFACTORY PARA TER TOOLSET UNICOS PARA CADA OPERACAO
# ------------------------------------------------------------------------------------------------

def parseFileName(input_string):
    parts = input_string.split(',')

    # Initialize variables
    n, p, r, t, v = None, None, None, None, None

    # Parse each part
    for part in parts:
        if part.startswith('n='):
            n = int(part.split('=')[1])
        elif part.startswith('p='):
            p = float(part.split('=')[1])
        elif part.startswith('r='):
            r = float(part.split('=')[1])
        elif part.startswith('t='):
            t = int(part.split('=')[1])
        else:
            v = part
    return n, p, r, t, v
    
def refactory():
    folder_path = './input/MyInstancesSameToolSets'
    files = os.listdir(folder_path)
    fileWithPath = [f"{folder_path}/{file}" for file in files]  
    
    toolUnusedMap = ld.loadToolSet("./input/UnusedToolSetsClean.csv")
    toolSetIndex = list(toolUnusedMap.keys())
    
    for file in fileWithPath:
        # load the file
        random.shuffle(toolSetIndex)
        numberOfJobs, priorityLevel, reentrantLevel, unicTools, version = parseFileName(file.split('/')[-1])
        
        toolSets = toolSetIndex[:int(numberOfJobs*(1+reentrantLevel))]
        instances = makeInstanceDiferentToolSets(numberOfJobs, reentrantLevel, priorityLevel, toolSets)
        
# ------------------------------------------------------------------------------------------------
# UTEIS
# ------------------------------------------------------------------------------------------------

def makeInstanceDiferentToolSets(numberOfJobs, reentrantRatio, priorityLevel, toolSets):
    toMakeReentrant = int(numberOfJobs/(1+reentrantRatio))
    toMakeReentrant = int(toMakeReentrant/2)
    
    jobsDict = []
    curToolSetIndex = 0
    for i in range(0, toMakeReentrant):
        jobsDict.append({
            "Job": i,
            "Operation": 0,
            "Processing Time": 1,
            "Priority": 0,
            
            "Reentrant": True,
            "ToolSet": [toolSets[curToolSetIndex], toolSets[curToolSetIndex+1]],
        })
        curToolSetIndex += 2
    
    for i in range(toMakeReentrant*2, numberOfJobs):
        jobsDict.append({
            "Job": i,
            "Operation": 0,
            "Processing Time": 1,
            "Priority": 0,
            
            "Reentrant": False,
            "ToolSet": [toolSets[curToolSetIndex]],
        })
        curToolSetIndex += 1
    
    random.shuffle(jobsDict)
    
    toMakePriority = int(numberOfJobs * priorityLevel)
    priorityCount = 0
    for i in range(0, len(jobsDict)):
        if priorityCount >= toMakePriority: break
        if jobsDict[i]['Reentrant']:
            jobsDict[i]['Priority'] = 1
            priorityCount += 2
        else:
            jobsDict[i]['Priority'] = 1
            priorityCount += 1

    # get len(jobsDict) processing times
    processingTime = geradorProcessingTime.generate_random_numbers(numberOfJobs)
    processingTimeIndex = 0
    realList = []
    for i, job in enumerate(jobsDict):
        if job['Reentrant']:
            realList.append({
                "Job": job['Job'],
                "Operation": 0,
                "Processing Time": processingTime[processingTimeIndex],
                "ToolSet": job['ToolSet'][0],
                "Priority": job['Priority'],

            })
            realList.append({
                "Job": job['Job'],
                "Operation": 1,
                "Processing Time": processingTime[processingTimeIndex+1],
                "ToolSet": job['ToolSet'][1],
                "Priority": job['Priority'],
            })
            processingTimeIndex += 2
        else:
            realList.append({
                "Job": job['Job'],
                "Operation": 0,
                "Processing Time": processingTime[processingTimeIndex],
                "ToolSet": job['ToolSet'][0],
                "Priority": job['Priority'],
            })
            processingTimeIndex += 1
    
    realList.sort(key=lambda x: x['Job'])
    
    saveInstances([{'instance': realList, 'size': len(realList), 'priority': f"{priorityLevel:.2f}", 'reentrant': reentrantRatio, 'unicTools': len(toolSets)}], "MyInstancesDiferentToolSets")

def makeInstance(quantidadeInstancias, reentrantRatio, priorityLivels, toolRatio, jobsDict, toolSetList):
    instancias = []

    # VARIANDO REENTRANTES
    for i in range(0, quantidadeInstancias):
        unicTools = set()
        newJobsDict = []
        
        listShuffle = [i for i in range(0, len(jobsDict))]
        random.shuffle(listShuffle)
        
        countTmp = 0
        
        ponto = int(len(jobsDict) * reentrantRatio)
        for j in range(0, ponto):
            jobsDict[listShuffle[j]]['Job'] = j
            jobsDict[listShuffle[j]]['Operation'] = 0
            jobsDict[listShuffle[j]]['Reentrant'] = True
            newJobsDict.append(jobsDict[listShuffle[j]])
            countTmp += 2

        for j in range(ponto, len(jobsDict)):
            jobsDict[listShuffle[j]]['Job'] = j
            jobsDict[listShuffle[j]]['Operation'] = 0
            jobsDict[listShuffle[j]]['Reentrant'] = False
            newJobsDict.append(jobsDict[listShuffle[j]])
            countTmp += 1
        
        # VARIAVEL PRIORIDADE
        for priorityLevel in priorityLivels:
            
            # fazer um shuffle de novo para nao ficar na mesma ordem
            random.shuffle(newJobsDict)

            ponto = int(countTmp * priorityLevel)
            numberOfPriority = 0
            for trem in newJobsDict:
                # print(trem)
                if numberOfPriority < ponto:
                    if trem['Reentrant']:
                        trem['Priority'] = 1
                        numberOfPriority += 2
                    else:
                        trem['Priority'] = 1
                        numberOfPriority += 1
                else:
                    trem['Priority'] = 0
            
            # ANALISANDO FERRAMENTAS UNICAS
            for toolRatioItem in toolRatio:
                for job in newJobsDict:
                    for tool in toolSetList[job['ToolSet']]:
                        unicTools.add(tool)
            
            # EXPANDIR JOBS
            definitiveJobs = []
            for i, job in enumerate(newJobsDict):
                if job['Reentrant'] == True:
                    copia = job.copy()
                    del copia['Reentrant']
                    time1, time2 = geradorProcessingTime.generate_random_numbers(2)
                    
                    instance1 = copia.copy()
                    instance1['Job'] = i
                    instance1['Operation'] = 0
                    instance1['Processing Time'] = time1
                    
                    instance2 = copia.copy()
                    instance2['Job'] = i
                    instance2['Operation'] = 1
                    instance2['Processing Time'] = time2
                    i
                    definitiveJobs.append(instance1)
                    definitiveJobs.append(instance2)
                else:
                    copia = job.copy()
                    del copia['Reentrant']
                    
                    copia['Job'] = i
                    copia['Operation'] = 0
                    definitiveJobs.append(copia)
            
            definitiveJobs.sort(key=lambda x: x['Job'])
            instancias.append({'instance': definitiveJobs, 'size': len(definitiveJobs), 'priority': f"{numberOfPriority/(len(definitiveJobs)):.2f}", 'reentrant': reentrantRatio, 'unicTools': len(unicTools)})
    
    return instancias

def makeJobs(toolSets):
    jobs = []
    processingTime = geradorProcessingTime.generate_random_numbers(len(toolSets))

    for i, toolSet in enumerate(toolSets):
        jobs.append({
            "Job": i,
            "Operation": 0,
            "ToolSet": toolSet,
            "Processing Time": processingTime[i],
        })
    
    return jobs

def saveInstances(instancias, folder="MyInstancesSameToolSets"):
    folder = f"/home/mateus/WSL/IC/SSP/input/{folder}"
    fields = ["Job","Operation","ToolSet","Processing Time","Priority"]
    for i, instancia in enumerate(instancias):
        fileName = f"n={instancia['size']},p={instancia['priority']},r={instancia['reentrant']},t={instancia['unicTools']},v{i}.csv"
            
        csv_file = open(f"{folder}/{fileName}", 'w', newline='')
        csv_writer = csv.DictWriter(csv_file, fieldnames=fields, delimiter=';')
        
        csv_writer.writeheader()
        csv_writer.writerows(instancia['instance'])

# ------------------------------------------------------------------------------------------------
# CONSTRUINDO INSTANCIAS PELAS BASE
# ------------------------------------------------------------------------------------------------

def makeInstaceBase():
    quantidadeInstancias = 1

    reentrantRatio = {
        '2M1376': 0.5,
        '6M1201': 0.6,
        '6M1401': 0.4,
    }

    priorityLivels = [0.25, 0.5, 0.75]

    toolRatio     = [1, 1.25, 1.50, 1.75, 2]

    toolSetList = ld.loadToolSet("ToolSetInt.csv")

    instancias376  = makeInstance(quantidadeInstancias, reentrantRatio['2M1376'], priorityLivels, toolRatio, ld.loadJobs("250Filtered.csv"),  toolSetList)
    instancias1201 = makeInstance(quantidadeInstancias, reentrantRatio['6M1201'], priorityLivels, toolRatio, ld.loadJobs("750Filtered.csv"),  toolSetList)
    instancias1401 = makeInstance(quantidadeInstancias, reentrantRatio['6M1401'], priorityLivels, toolRatio, ld.loadJobs("1000Filtered.csv"), toolSetList)

    return instancias376, instancias1201, instancias1401

# ------------------------------------------------------------------------------------------------
# CONSTRUINDO INSTANCIAS EXTRAS
# ------------------------------------------------------------------------------------------------

def makeInstaceExtra():
    instanciasToReturn = []

    toolUnusedMap = ld.loadToolSet("./input/UnusedToolSetsClean.csv")
    toolSetIndex = list(toolUnusedMap.keys())

    quantidadeInstancias = 1
    reentrantRatio = {
        376: 0.5,
        1201: 0.6,
        1401: 0.4,
    }
    priorityLivels = [0.25, 0.5, 0.75]
    toolRatio     = [1, 1.25, 1.50, 1.75, 2]

    random.shuffle(toolSetIndex)

    for i in range(400, min(len(toolSetIndex), 1100), 100):
        closer = min(reentrantRatio, key=lambda x:abs(x-i))
        closer = reentrantRatio[closer]
        corte = int((i) / (1+closer))

        print(f"{closer} | {i} | {corte}")

        toolSets = toolSetIndex[:corte]
        jobs = makeJobs(toolSets)
        instancias = makeInstance(quantidadeInstancias, closer, priorityLivels, toolRatio, jobs, toolUnusedMap)
        instanciasToReturn += instancias

    maxToolSets = 1238
    # maxToolSets = 1266
    closer = min(reentrantRatio, key=lambda x:abs(x-maxToolSets))
    closer = reentrantRatio[closer]
    corte = int((maxToolSets) / (1+closer))

    print(f"{closer} | {maxToolSets} | {corte}")

    toolSets = toolSetIndex[:corte]
    jobs = makeJobs(toolSets)
    instancias = makeInstance(quantidadeInstancias, closer, priorityLivels, toolRatio, jobs, toolUnusedMap)
    instanciasToReturn += instancias

    return instanciasToReturn

# ------------------------------------------------------------------------------------------------
# SALVANDO INSTANCIAS
# ------------------------------------------------------------------------------------------------

# instancias376, instancias1201, instancias1401 = makeInstaceBase()
# saveInstances(instancias376 + instancias1201 + instancias1401)
        
# instances = makeInstaceExtra()
# saveInstances(instances)

# refactory()

# createSmallerInstances()
# creackTools()
makeSmallDiff()

# toolUnusedMap = ld.loadToolSet("./input/UnusedToolSetsClean.csv")
# toolSetIndex = list(toolUnusedMap.keys())
# print(toolSetIndex)


class DatasetCreator:
    def __init__(self, dataset):
        self.dataset = dataset