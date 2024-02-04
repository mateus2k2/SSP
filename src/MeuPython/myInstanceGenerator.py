import pandas as pd
import random
import csv


# ------------------------------------------------------------------------------------------------
# PARAMETROS
# ------------------------------------------------------------------------------------------------

reentrantRatio = {
    '2M1376': 0.5,
    '6M1201': 0.6,
    '6M1401': 0.4,
}

quantidadeInstancias = 5

# priorityLivels = [0, 0.125, 0.25, 0.375, 0.5, 0.625, 0.75]
priorityLivels = [0.25, 0.5, 0.75]

toolRatio     = [1, 1.25, 1.50, 1.75, 2]


# ------------------------------------------------------------------------------------------------
# CARREGAR DADOS
# ------------------------------------------------------------------------------------------------

def loadToolSet():
    toolSetsFilePath = '/home/mateus/WSL/IC/data/ToolSetInt.csv'
    toolSetsDF = pd.read_csv(toolSetsFilePath, header=None, delimiter=';')
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

def loadJobs(number):
    jobsFilePath = f'/home/mateus/WSL/IC/data/{number}.csv'
    jobsDF = pd.read_csv(jobsFilePath, delimiter=';')
    jobsDict = jobsDF.to_dict(orient='records')
    
    return jobsDict

# ------------------------------------------------------------------------------------------------
# CREATING
# ------------------------------------------------------------------------------------------------

def makeInstance(reentrantRatio, jobsDict, toolSetList):
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
                        trem['Priority'] = True
                        numberOfPriority += 2
                    else:
                        trem['Priority'] = True
                        numberOfPriority += 1
                else:
                    trem['Priority'] = False
            
            # ANALISANDO FERRAMENTAS UNICAS
            for toolRatioItem in toolRatio:
                for job in newJobsDict:
                    for tool in toolSetList[job['ToolSet']-1]:
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

# ------------------------------------------------------------------------------------------------
# CONSTRUINDO INSTANCIAS
# ------------------------------------------------------------------------------------------------


toolSetList = loadToolSet()

instancias376  = makeInstance(reentrantRatio['2M1376'], loadJobs("250Filtered"),  toolSetList)
# instancias1201 = makeInstance(reentrantRatio['6M1201'], loadJobs("750Filtered"),  toolSetList)
# instancias1401 = makeInstance(reentrantRatio['6M1401'], loadJobs("1000Filtered"), toolSetList)

instancias = instancias376 # + instancias1201 + instancias1401

print(len(instancias))

# ------------------------------------------------------------------------------------------------
# SALVANDO INSTANCIAS
# ------------------------------------------------------------------------------------------------

folder = "/home/mateus/WSL/IC/data/MyInstances"
fields = ["Job","Operation","ToolSet","Processing Time","Priority"]
for i, instancia in enumerate(instancias):
    fileName = f"n={instancia['size']},p={instancia['priority']},r={instancia['reentrant']},t={instancia['unicTools']},v{i}.csv"
        
    csv_file = open(f"{folder}/{fileName}", 'w', newline='')
    csv_writer = csv.DictWriter(csv_file, fieldnames=fields)
    
    csv_writer.writeheader()
    csv_writer.writerows(instancia['instance'])