from collections import defaultdict, deque
from heapq import nsmallest
import sys
import threading

# -------------------------------------------------------
# CALCULO DA QUANTIDADE DE TRACAS DE FERRAMENTAS
# https://codereview.stackexchange.com/questions/193371/python-methods-to-reduce-running-times-ktns-algorithm
# -------------------------------------------------------

def KTNS(schedule, toolSets, toolSetNeaded, capacity):

    # Dicionario do tipo: Chave = Ferramentas e Valor = Lista de deque (Double Ended Queue). As deque tem os jobs que uma dada ferrementa vai ser usada
    toolInstants = defaultdict(deque)
    for i, job in enumerate(schedule): # Para cara job no schedule
        for tool in toolSets[toolSetNeaded[job]-1]: # Pega o conjunto de ferramentas que o job precisa
            toolInstants[tool].append(i) # Adiciona o job no deque da ferramenta
    
    for instants in toolInstants.values():
        # Adiciona o tamanho do schedule no final de cada deque
        # instants.append(sys.maxsize)
        instants.append(len(schedule))
        
    def nextNeeded(tool):
        return toolInstants[tool][0]

    # Representa as ferramentas que estao na maquina no momento
    # Inicializa o set com as ferramentas que vao ser usadas nos primeiros jobs
    # nsmallest retorna os n (capacity) menores elementos da lista baseado na funcao nextNeeded
    # Ferramentas com menor nextNeeded sao as que vao ser usadas primeiro
    machine = set(nsmallest(capacity, toolInstants, key=nextNeeded))

    variableCost = len(machine)
    fixedCost = 0
    string = ''
    for job in schedule:
        
        # print(f'Job: {job+1} machine: {machine}')
        string += f'Job: {job+1} machine: {machine}\n'
        
        # Ferramentas que vao ser usadas nesse job e nao estao na maquina
        new_tools = {tool for tool in toolSets[toolSetNeaded[job]-1] if tool not in machine}
        if new_tools:
            
            fixedCost += 1
            variableCost += len(new_tools)
            
            # Remove ferramentas ate o tamanho do set ser igual a capacidade
            for _ in range(len(machine) + len(new_tools) - capacity):
                machine.remove(max(machine, key=nextNeeded)) # Max itera sobre o set e retorna o elemento com maior nextNeeded e o Remove deleta esse elemento do set
            machine.update(new_tools) # Adiciona as novas ferramentas que vao ser usadas nesse job ao set machine
        
        for tool in toolSets[toolSetNeaded[job]-1]: # Pega o conjunto de ferramentas desse job
            toolInstants[tool].popleft() # Remove o job do deque da ferramenta
    
    return (variableCost, fixedCost, string)


# -------------------------------------------------------
# TOTAL COST
# -------------------------------------------------------

def Totalcost(priorityCount, toolSets, toolSetNeaded, machineVector):
    unfinishedPriority = priorityCount[0] - priorityCount[1]
    fineshedOperations = len(machineVector)
    toolSwitchVariable = 0
    toolSwitchFixed = 0

    machineCount = len(set(machineVector)) 
    
    schedules = [[] for _ in range(machineCount)] 
    for i in range(len(machineVector)):
        schedules[machineVector[i]-1].append(i) 
    
    capacity = max(len(lst) for lst in toolSets)        
    for i, schedule in enumerate(schedules):
        variable, fixed, string = KTNS(schedule, toolSets, toolSetNeaded, capacity)

        print(f'Schedule: {i+1}')
        print(string)
        print(f'Variavel:{variable}, Fixo:{fixed}\n')
        
        toolSwitchVariable += variable
        toolSwitchFixed += fixed
        
        fineshedOperations += len(schedule)
        

    return fineshedOperations - (toolSwitchFixed + toolSwitchVariable + unfinishedPriority)

# -------------------------------------------------------
# TOTAL COST WITH THREADS
# -------------------------------------------------------

def TotalcostThreads(priorityCount, toolSets, toolSetNeeded, machineVector):
    unfinishedPriority = priorityCount[0] - priorityCount[1]
    finishedOperations = len(machineVector)
    toolSwitchVariable = 0
    toolSwitchFixed = 0

    machineCount = len(set(machineVector)) 
    
    schedules = [[] for _ in range(machineCount)] 
    for i in range(len(machineVector)):
        schedules[machineVector[i]-1].append(i) 
    
    capacity = max(len(lst) for lst in toolSets)        
    threadResults = {}

    def worker(schedule, toolSets, toolSetNeeded, capacity, machineIndex):
        variable, fixed, stringReturn = KTNS(schedule, toolSets, toolSetNeeded, capacity)
        threadResults[machineIndex] = {'Variable': variable, 'Fixed': fixed, 'String': stringReturn}

    threads = []
    for i, schedule in enumerate(schedules):
        thread = threading.Thread(target=worker, args=(schedule, toolSets, toolSetNeeded, capacity, i+1))
        threads.append(thread)
        thread.start()

    for thread in threads:
        thread.join()

    for i in range(len(schedules)):
        result = threadResults[i+1]
        
        print(f'Schedule: {i+1}')
        print(result["String"])        
        print(f'Variavel:{result["Variable"]}, Fixo:{result["Fixed"]}\n')
        
        toolSwitchVariable += result["Variable"]
        toolSwitchFixed += result["Fixed"]

    return finishedOperations - (toolSwitchFixed + toolSwitchVariable + unfinishedPriority)
    