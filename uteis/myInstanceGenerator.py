import pandas as pd
import random
import csv
import loadData as ld
from ProcessingTimeGenerator import ProcessingTimeGenerator
import os

geradorProcessingTime = ProcessingTimeGenerator()

# ------------------------------------------------------------------------------------------------
# REFACTORY PARA TER TOOLSET UNICOS PARA CADA OPERACAO
# ------------------------------------------------------------------------------------------------

def refactory():
    folder_path = '/home/mateus/WSL/IC/SSP/input/MyInstancesDiferentToolSets'
    files = os.listdir(folder_path)
    fileWithPath = [f"{folder_path}/{file}" for file in files]  
    
    for file in fileWithPath:
        # load the file
        df = pd.read_csv(file, delimiter=';')
        
        # get the toolSets
        toolSets = df['ToolSet'].unique()
        toolSets = list(toolSets)
        
        # iterate over each line of the df
        # and change the toolSet to a new one
        deleteOver = 0
        for i, row in df.iterrows():
            if len(toolSets) == 0: break
            deleteOver = i
            newToolSet = toolSets[0]
            toolSets = toolSets[1:]
            df.at[i, 'ToolSet'] = newToolSet
        
        #delete all the lines that are over the DeleteOver index
        df = df.drop(df.index[deleteOver+1:])
        
        # save the file
        df.to_csv(file, sep=';', index=False)

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

def saveInstances(instancias):
    folder = "/home/mateus/WSL/IC/SSP/input/MyInstancesSameToolSets"
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

    toolUnusedMap = ld.loadToolSet("UnusedToolSetsClean.csv")
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

instancias376, instancias1201, instancias1401 = makeInstaceBase()
saveInstances(instancias376 + instancias1201 + instancias1401)
        
instances = makeInstaceExtra()
saveInstances(instances)

# refactory()

# toolUnusedMap = ld.loadToolSet("UnusedToolSetsClean.csv")
# toolSetIndex = list(toolUnusedMap.keys())
# print(toolSetIndex)