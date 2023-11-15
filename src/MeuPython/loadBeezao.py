def loadBeezaoInstance(cominho):
    file = open(cominho, 'r')
    lines = file.readlines()

    machines, jobs, tools, capacity = map(int, lines[0].split())

    switchingTime = int(lines[1])

    processingTimes = list(map(int, lines[2].split()))
    
    toolMatrix = [list(map(int, line.split())) for line in lines[3:]][:-1]
    toolMatrixTraspose = list(map(list, zip(*toolMatrix)))

    toolSets = [[] for _ in range(jobs)]
    for i, set in enumerate(toolMatrixTraspose):
        for j, tool in enumerate(set):
            if tool != 0:
                toolSets[i].append(j+1)
                
    jobsVector = list(range(1, jobs + 1))
    toolsSetNeaded = list(range(1, jobs + 1))
        
        
    return machines, capacity, switchingTime, processingTimes, jobsVector, toolsSetNeaded, toolSets


def loadBeezaoInstanceOG(cominho):
    file = open(cominho, 'r')
    lines = file.readlines()

    machines, jobs, tools, capacity = map(int, lines[0].split())

    switchingTime = int(lines[1])

    processingTimes = list(map(int, lines[2].split()))
    
    toolMatrix = [list(map(int, line.split())) for line in lines[3:]][:-1]
    
    return machines, jobs, tools, capacity, switchingTime, processingTimes, toolMatrix