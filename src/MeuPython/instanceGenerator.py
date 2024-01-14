# 2M376 6M1201 6M1401
# 250   750    1000

# Job;Item;TL;Processing Time
# Job;Operation;ToolSet;Processing Time

#TL e um tool set com varias tool dentro dele

import pandas as pd

# ------------------------------------------------------------------------------------------------
# DADOS
# ------------------------------------------------------------------------------------------------

toolSetsFilePath = '/home/mateus/WSL/IC/data/ToolSetInt.csv'
toolSetsDF = pd.read_csv(toolSetsFilePath, delimiter=';')
toolSetsDF = toolSetsDF.drop(toolSetsDF.columns[0], axis=1)
toolSetsDF = toolSetsDF.fillna('NaNPlaceholder')
toolSetsDict = toolSetsDF.to_dict(orient='records')
cleanToolSetsDict = []
for i in range(0, len(toolSetsDict)):
    cleanToolSetsDict.append([])
    for coisa in toolSetsDict[i].values():
        if coisa != 'NaNPlaceholder':
            cleanToolSetsDict[i].append(int(coisa))

jobsFilePath = '/home/mateus/WSL/IC/data/250.csv'
jobsDF = pd.read_csv(jobsFilePath, delimiter=';')

original = jobsDF.to_dict(orient='records')
modificado = []

# ------------------------------------------------------------------------------------------------
# PARAMETROS
# ------------------------------------------------------------------------------------------------

reentrantRatio = {
    '2M1': 0.5,
    '6M1': 0.6,
    '6M2': 0.4,
}

maxJobs = {
    '2M1':   376,
    '6M1':   1201,
    '6M2':   1401,
}

t_min = {
    '2M1':   2,
    '6M1':   10,
    '6M2':   5,
}

priorityLevel = {
    '2M1':   0.53,
    '6M1':   0.51,
    '6M2':   0.49,
}

toolRatio = {
    '2M1':   1.73,
    '6M1':   1.27,
    '6M2':   1.0,
}

# ------------------------------------------------------------------------------------------------
# OPERATIONS
# ------------------------------------------------------------------------------------------------

countNumerJobs1 = 0
countNumerJobs2 = 0

def calculatioNumberOfSetups(countNumerJobs2, numeroAtual):
    # conta = countNumerJobs2 / (numeroAtual-countNumerJobs2 + 1)
    conta = countNumerJobs2 / (maxJobs['2M1']-countNumerJobs2 + 1)
    
    if conta > reentrantRatio['2M1']:
        return 1
    else:
        return 2


for i in range(0, len(original)):
    if len(modificado) >= maxJobs['2M1']:
        break 

    if calculatioNumberOfSetups(countNumerJobs2, len(modificado)) == 1:
        countNumerJobs1 += 1
        modificado.append(original[i])
    else:
        countNumerJobs2 += 1
        modificado.append(original[i])
        modificado.append(original[i])

print("len(original) "  , len(original))
print("len(modificado) ", len(modificado))
print("countNumerJobs2 ", countNumerJobs2)
print("countNumerJobs1 ", countNumerJobs1)

# ------------------------------------------------------------------------------------------------
# PRIORITY
# ------------------------------------------------------------------------------------------------

numberOfPriority = 0
numberOfRegular = 0

def calculatioPriority(numberOfPriority, numberOfRegular):
    conta = (numberOfPriority / (numberOfPriority + numberOfRegular + 1))
    
    if conta < priorityLevel['2M1']:
        return True
    else:
        return False

for i in range(0, len(modificado)):
    
    if calculatioPriority(numberOfPriority, numberOfRegular):
        modificado[i]['Priority'] = 1
        numberOfPriority += 1
    else:
        modificado[i]['Priority'] = 0
        numberOfRegular += 1


# print("numberOfPriority ", numberOfPriority)
# print("numberOfRegular ", numberOfRegular)

# ------------------------------------------------------------------------------------------------
# TOOL RATIO
# ------------------------------------------------------------------------------------------------

uniqueTools = set()

for job in modificado:
    toolSetIndex = job['ToolSet']
    for tool in cleanToolSetsDict[toolSetIndex-2]:
        uniqueTools.add(tool)

# print(len(uniqueTools))

# ------------------------------------------------------------------------------------------------
# TESTES
# ------------------------------------------------------------------------------------------------

