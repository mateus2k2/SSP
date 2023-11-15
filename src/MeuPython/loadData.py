import random
import pandas as pd
import json


def optimizeToolSetCSV():
    df = pd.read_csv('/home/mateus/WSL/IC/data/Data Tool Set Compositions.csv', sep=';', header=None)

    df = df.fillna('NaNPlaceholder')
    df = df.drop(0, axis=1)
    df.index = range(1, len(df) + 1)
    
    uniqueStrings = df.stack().unique()
    stringToIntMapping = {string: i+1 for i, string in enumerate(uniqueStrings)}
    stringToIntMapping['NaNPlaceholder'] = ''

    df = df.map(lambda x: stringToIntMapping[x])

    df.to_csv('/home/mateus/WSL/IC/data/Data Tool Set Compositions Int.csv', index=True, header=False, sep=';')
    
    with open('data.json', 'w') as fp:
        json.dump(stringToIntMapping, fp)


    print(stringToIntMapping)
    
# ---------------------------------------------------------------
# LOADING JOBS DATA
# ---------------------------------------------------------------

def loadJobs(fileName):
    JobsAll = pd.read_csv(fileName, delimiter=';', dtype=int)

    jobs = []
    operations = []
    toolSetNeaded = []
    processingTime = []

    for i in range(len(JobsAll)):
        jobs.append(JobsAll['Job'][i])
        operations.append(JobsAll['Operation'][i])
        toolSetNeaded.append(JobsAll['ToolSet'][i])
        processingTime.append(JobsAll['Processing Time'][i])
    
    return jobs, operations, toolSetNeaded, processingTime   
    
# ---------------------------------------------------------------
# LOADING TOOLS DATA
# ---------------------------------------------------------------

def loadTools(fileName):
    ToolSetAll = pd.read_csv(fileName, delimiter=';',header=None, dtype=str)

    ToolSetAll = ToolSetAll.drop(ToolSetAll.columns[0], axis=1)
    toolSets = ToolSetAll.values.tolist()
    toolSets = [[int(x) for x in row if x == x] for row in toolSets]
    return toolSets

# ---------------------------------------------------------------
# SETING PRIORITY AND MACHINEVECTOR TO 0
# ---------------------------------------------------------------

def makePriorityAndMachine(size, priority = 1, machine = 1, randomMachines = False):
    priorityVector = [ priority for _ in range(size)]
    
    if randomMachines:
        machineVector = [ random.randint(1, machine) for _ in range(size)]
    else:
        machineVector = [ machine for _ in range(size)]
        
    return priorityVector, machineVector

# ---------------------------------------------------------------
# LOADING TEST SOLUTION EXEMPLE 1
# ---------------------------------------------------------------

def testSolution1():
    # Tools Sets exemple 1
    toolSets = [
        [1,2,3,4,5],
        [12,13,14,15,16,17,18],
        [4,5,8,9,10,11,12,13],
        [5,6,7],
        [15,16,17,18,19,20],
    ]

    # Exemple 1 - TABELA
    jobs           = [1,1,2,3,3,4,4,5,6,7]
    operations     = [1,2,1,1,2,1,2,1,1,1]
    priority       = [1,1,0,0,0,1,1,0,0,1]
    toolSetNeaded  = [1,1,2,3,3,2,2,4,5,1]
    machineVector  = [1,1,1,2,2,2,2,2,2,2]

    priorityBefone = priority.count(1)

    # Exemple 1 - SOLUTION
    jobs           = [1,1,2,3,3,4,4,5,6]
    operations     = [1,2,1,1,2,1,2,1,1]
    priority       = [1,1,0,0,0,1,1,0,0]
    toolSetNeaded  = [1,1,2,3,3,2,2,4,5]
    machineVector  = [1,1,1,1,1,2,2,2,2]

    priorityAfter = priority.count(1)
    
    return toolSets, jobs, operations, priority, toolSetNeaded, machineVector, priorityBefone, priorityAfter


def testSolution2():
    # Tools Sets exemple 1
    toolSets = [
        [1,2,3,4,5],
        [12,13,14,15,16,17,18],
        [4,5,8,9,10,11,12,13],
        [5,6,7],
        [15,16,17,18,19,20],
    ]

    # Exemple 1 - TABELA
    jobs           = [1,1,2,3,3,4,4,5,6,7]
    operations     = [1,2,1,1,2,1,2,1,1,1]
    priority       = [1,1,0,0,0,1,1,0,0,1]
    toolSetNeaded  = [1,1,2,3,3,2,2,4,5,1]
    machineVector  = [1,1,1,2,2,2,2,2,2,2]

    priorityBefone = priority.count(1)

    # Exemple 1 - SOLUTION
    jobs           = [1,1,2,4,4,3,3,5,6]
    operations     = [1,2,1,1,2,1,2,1,1]
    priority       = [1,1,0,1,1,0,0,0,0]
    toolSetNeaded  = [1,1,2,2,2,3,3,4,5]
    machineVector  = [1,1,1,1,1,1,1,2,2]

    priorityAfter = priority.count(1)
    
    return toolSets, jobs, operations, priority, toolSetNeaded, machineVector, priorityBefone, priorityAfter


def testSolution3():
    # Tools Sets exemple 1
    toolSets = [
        [1,2,3,4,5],
        [12,13,14,15,16,17,18],
        [4,5,8,9,10,11,12,13],
        [5,6,7],
        [15,16,17,18,19,20],
    ]

    # Exemple 1 - TABELA
    jobs           = [1,1,2,3,3,4,4,5,6,7]
    operations     = [1,2,1,1,2,1,2,1,1,1]
    priority       = [1,1,0,0,0,1,1,0,0,1]
    toolSetNeaded  = [1,1,2,3,3,2,2,4,5,1]
    machineVector  = [1,1,1,2,2,2,2,2,2,2]

    priorityBefone = priority.count(1)

    # Exemple 1 - SOLUTION
    jobs           = [2,4,4,3,3,5,6,1,1,7]
    operations     = [1,1,2,1,2,1,1,1,2,1]
    priority       = [0,1,1,0,0,0,0,1,1,1]
    toolSetNeaded  = [2,2,2,3,3,4,5,1,1,1]
    machineVector  = [1,1,1,1,1,2,2,2,2,2]

    priorityAfter = priority.count(1)
    
    return toolSets, jobs, operations, priority, toolSetNeaded, machineVector, priorityBefone, priorityAfter


def testSolution4():
    # Tools Sets exemple 1
    toolSets = [
        [1,2,3,4,5],
        [12,13,14,15,16,17,18],
        [4,5,8,9,10,11,12,13],
        [5,6,7],
        [15,16,17,18,19,20],
    ]

    # Exemple 1 - TABELA
    jobs           = [1,1,2,3,3,4,4,5,6,7]
    operations     = [1,2,1,1,2,1,2,1,1,1]
    priority       = [1,1,0,0,0,1,1,0,0,1]
    toolSetNeaded  = [1,1,2,3,3,2,2,4,5,1]
    machineVector  = [1,1,1,2,2,2,2,2,2,2]

    priorityBefone = priority.count(1)

    # Exemple 1 - SOLUTION
    jobs           = [1,1,7,2,4,4,5,6,3,3]
    operations     = [1,2,1,1,1,2,1,1,1,2]
    priority       = [1,1,1,0,1,1,0,0,0,0]
    toolSetNeaded  = [1,1,1,2,2,2,4,5,3,3]
    machineVector  = [1,1,1,1,1,1,2,2,2,2]

    priorityAfter = priority.count(1)
    
    return toolSets, jobs, operations, priority, toolSetNeaded, machineVector, priorityBefone, priorityAfter
