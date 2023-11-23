import cost as c
import loadData as ld
import loadBeezao as lb

def myPrint(listOfThings):
    for thing in listOfThings:
        print(f'{thing[0]}')
        if isinstance(thing[1], list) and all(isinstance(sublist, list) for sublist in thing[1]):
            (print(row) for row in thing[1])
        else:
            print(thing[1])
        print()

# ---------------------------------------------------------------
# LOADING BEEZAO INSTANCE 
# ---------------------------------------------------------------

# caminhoArquivo = '/home/mateus/WSL/IC/pmstsup/Beezao Instances/Beezao instances [IPMTC-I]/instance1_m=2_n=8_l=15_c=5_s=0.PMTC'
# machines, jobs, tools, capacity, switchingTime, processingTimes, toolMatrix = lb.loadBeezaoInstanceOG(caminhoArquivo)
# myPrint([("Machines", machines), ("Jobs", jobs), ("Tools", tools), ("Capacity", capacity), ("Switching Time", switchingTime), ("Processing Times", processingTimes), ("Tool Matrix", toolMatrix)])

# caminhoArquivo = '/home/mateus/WSL/IC/pmstsup/Beezao Instances/Beezao instances [IPMTC-I]/instance1_m=2_n=8_l=15_c=5_s=0.PMTC'
# machines, capacity, switchingTime, processingTimes, jobsVector, toolsSetNeaded, toolSets = lb.loadBeezaoInstance(caminhoArquivo)
# myPrint([("Machines", machines), ("Capacity", capacity), ("Switching Time", switchingTime), ("Processing Times", processingTimes), ("Jobs Vector", jobsVector), ("Tools Set Neaded", toolsSetNeaded), ("Tool Sets", toolSets)])

# ---------------------------------------------------------------
# LOADING AND PRINTING DATA 
# ---------------------------------------------------------------

# jobs, operations, toolSetNeaded, processingTime = ld.loadJobs('/home/mateus/WSL/IC/data/Data Jobs 1000.csv')
# toolSets = ld.loadTools('/home/mateus/WSL/IC/data/Data Tool Set Compositions.csv')
# priority, machineVector = ld.makePriorityAndMachine(size = len(jobs), priority = 1, machine = 8, randomMachines=True)
# priorityBefone, priorityAfter = [0, 0]
# myPrint([("Jobs", jobs), ("Operations", operations), ("toolSetNeded", toolSetNeaded), ("Priority", priority), ("MachineVector", machineVector), ("processingTime", processingTime), ("ToolSets", toolSets)])

# toolSets, jobs, operations, priority, toolSetNeaded, machineVector, priorityBefone, priorityAfter = ld.testSolution4()
# myPrint([("Jobs", jobs), ("Operations", operations), ("toolSetNeded", toolSetNeaded), ("Priority", priority), ("MachineVector", machineVector), ("ToolSets", toolSets)])

# ---------------------------------------------------------------
# CALCULATING COST
# ---------------------------------------------------------------

# print("\n------------------------------------------------------------------------------------------------------------------------------------------------------------------\n")
# print("Custo Total:", c.Totalcost((priorityBefone, priorityAfter), toolSets, toolSetNeaded, machineVector)) 