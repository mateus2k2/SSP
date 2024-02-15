import pandas as pd
import random
import csv
import loadData as ld
from uteis import ProcessingTimeGenerator

geradorProcessingTime = ProcessingTimeGenerator()

# ------------------------------------------------------------------------------------------------
# UTEIS
# ------------------------------------------------------------------------------------------------

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
                    
                    instance1 = copia.copy()
                    instance1['Job'] = i
                    instance1['Operation'] = 0
                    
                    instance2 = copia.copy()
                    instance2['Job'] = i
                    instance2['Operation'] = 1
                    
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

def saveInstances(instancias):
    folder = "/home/mateus/WSL/IC/data/MyInstances"
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

    toolUnusedMap = ld.loadToolSet("UnusedToolSets.csv")
    toolSetIndex = list(toolUnusedMap.keys())

    quantidadeInstancias = 1
    reentrantRatio = {
        376: 0.5,
        1201: 0.6,
        1401: 0.4,
    }
    priorityLivels = [0.25, 0.5, 0.75]
    toolRatio     = [1, 1.25, 1.50, 1.75, 2]

    for i in range(400, min(len(toolSetIndex), 1100), 100):
        closer = min(reentrantRatio, key=lambda x:abs(x-i))
        closer = reentrantRatio[closer]
        corte = int(i - (closer * i))

        # print(f"{closer} | {i} | {corte}")

        random.shuffle(toolSetIndex)
        toolSets = toolSetIndex[:i-corte]
        jobs = makeJobs(toolSets)
        instancias = makeInstance(quantidadeInstancias, closer, priorityLivels, toolRatio, jobs, toolUnusedMap)
        instanciasToReturn += instancias

    for i in range(1250, len(toolSetIndex), 250):
        pass

    return instanciasToReturn

# ------------------------------------------------------------------------------------------------
# SALVANDO INSTANCIAS
# ------------------------------------------------------------------------------------------------

instancias376, instancias1201, instancias1401 = makeInstaceBase()
saveInstances(instancias376 + instancias1201 + instancias1401)
        
# instances = makeInstaceExtra()
# saveInstances(instances)

# ssh-add ~/.ssh/id_ed25519
# chmod  400 ~/.ssh/id_ed25519      