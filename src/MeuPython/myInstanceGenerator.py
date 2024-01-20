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
        
        ponto = int(len(jobsDict) * reentrantRatio)
        for j in range(0, ponto):
            newJobsDict.append(jobsDict[listShuffle[j]])
            newJobsDict.append(jobsDict[listShuffle[j]])
        for j in range(ponto, len(jobsDict)): 
            newJobsDict.append(jobsDict[listShuffle[j]])
        
        # VARIAVEL PRIORIDADE
        for priorityLevel in priorityLivels:
            
            ponto = int(len(newJobsDict) * priorityLevel)
            for j in range(0, ponto):                newJobsDict[j]['Priority'] = 1
            for j in range(ponto, len(newJobsDict)): newJobsDict[j]['Priority'] = 0
            
            #ANALISANDO FERRAMENTAS UNICAS
            for toolRatioItem in toolRatio:
                
                for job in newJobsDict:
                    for tool in toolSetList[job['ToolSet']-2]:
                        unicTools.add(tool)

            instancias.append({'instance': newJobsDict, 'size': len(newJobsDict), 'priority': priorityLevel, 'reentrant': reentrantRatio, 'unicTools': len(unicTools)})
            
    return instancias

# ------------------------------------------------------------------------------------------------
# CONSTRUINDO INSTANCIAS
# ------------------------------------------------------------------------------------------------


toolSetList = loadToolSet()

instancias376 = makeInstance(reentrantRatio['2M1376'], loadJobs(250),  toolSetList)
instancias1201 = makeInstance(reentrantRatio['6M1201'], loadJobs(750),  toolSetList)
instancias1401 = makeInstance(reentrantRatio['6M1401'], loadJobs(1000), toolSetList)

instancias = instancias376 + instancias1201 + instancias1401

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