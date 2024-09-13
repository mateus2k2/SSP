import loadData as ld
import sys
import reportParser as rp

# ---------------------------------------------------------------------------------------------------
# VALIDADOR
# ---------------------------------------------------------------------------------------------------

def jobLookup(jobs, job, operation, returnIndex=False):
    for index, j in enumerate(jobs):
        if j['Job'] == job and j['Operation'] == operation:
            if returnIndex: return j, index
            else: return j
    return None

def checkMagazine (machines, toolSets, jobs):
    print(f"Checking Magazine")

    for i, machine in enumerate(machines):
        print(f"Machine {i+1}/{len(machines)}")
        error = False
        for j, operation in enumerate(machine['operations']):
            realJob = jobLookup(jobs, operation['job'], operation['operation'])

            if realJob == None:
                print(f"Error = Job {operation['job']} Operation {operation['operation']} not found in the instance")
                error = True
            
            if not set(toolSets[realJob['ToolSet']]).issubset(set(operation['magazine'])):
                print(f"Error = Job {operation['job']} Operation {operation['operation']} ToolSet {realJob['ToolSet']} not found in magazine")
                error = True

        if error:
            print("ERROR")

        else: 
            print("OK")

def checkUnsupervisedSwitchs(machines, toolSets, jobs, planejamento):
    print("Checking Unsupervised Switchs")

    unsupervisedStart = planejamento['unsupervised']
    timeScale = planejamento['timescale']

    for i, machine in enumerate(machines):
        print(f"Machine {i+1}/{len(machines)}")

        curTime = 0
        error = False

        for j, operation in enumerate(machine['operations']):
            curJob = jobLookup(jobs, operation['job'], operation['operation'])
            curToolSet = toolSets[curJob['ToolSet']]

            lastJob = jobLookup(jobs, machine['operations'][j-1]['job'], machine['operations'][j-1]['operation'])
            lastToolSet = toolSets[lastJob['ToolSet']]

            curTime = operation['start']

            if curTime%timeScale >= unsupervisedStart:
                if operation['magazine'] != machine['operations'][j-1]['magazine'] and len(set(curToolSet) - set(lastToolSet)) > 0:
                    print(f"Error = Unsupervised Switch in Machine {i+1}/{len(machines)} | Job {operation['job']} Operation {operation['operation']}")
                    error = True
            
        if error:
            print("ERROR")
        else:
            print("OK")

def checkSwitchs(machines, toolSets, jobs):
    print("Checking Switchs")

    for i, machine in enumerate(machines):
        print(f"Machine {i+1}/{len(machines)}")

        switchInstance = 0
        toolSwitchs = 0

        error = False

        for j, operation in enumerate(machine['operations']):

            if j == 0:
                switchInstance += 1
                toolSwitchs += len(operation['magazine'])
            else:
                if operation['magazine'] != machine['operations'][j-1]['magazine']:
                    switchInstance += 1
                    toolSwitchs += len(set(operation['magazine']) - set(machine['operations'][j-1]['magazine']))
        
        if (switchInstance != machine['end_info']['switchsInstances'] or toolSwitchs != machine['end_info']['switchs']):
            print(f"Error = Found {switchInstance} switchInstances and {toolSwitchs} toolSwitchs, expected {machine['end_info']['switchsInstances']} switchInstances and {machine['end_info']['switchs']} toolSwitchs")
            error = True
        
        if error:
            print("ERROR")
        else:
            print("OK")

def checkUnfinishedJobs(machines, jobs):
    print("Checking Unfinished Jobs")

    for i, machine in enumerate(machines):
        print(f"Machine {i+1}/{len(machines)}")

        numberOfPriorityJobsExpected = 0
        numerOfFinishedPriorityJobs = 0

        error = False

        for job in machine["machine_info"]:
            realJob = jobLookup(jobs, job['job'], job['operation'])
            if realJob['Priority'] == 1: numberOfPriorityJobsExpected += 1

        for operation in machine['operations']:
            realJob = jobLookup(jobs, operation['job'], operation['operation'])
            if realJob['Priority'] == 1 and operation['priority'] == 1: numerOfFinishedPriorityJobs += 1            

        if (numberOfPriorityJobsExpected != numerOfFinishedPriorityJobs):
            print(f"Error = Found {numerOfFinishedPriorityJobs} finished priority jobs, expected {numberOfPriorityJobsExpected}")
            error = True
        
        if error:
            print("ERROR")
        else:
            print("OK")

def checkProfit(machines, jobs, planejamento):
    print("Checking Profit")

    acumulatorFound = 0

    for i, machine in enumerate(machines):
        print(f"Machine {i+1}/{len(machines)}")

        error = False

        cost = machine['end_info']['cost']
        conta = (30 * machine['end_info']['fineshedPriorityCount']) - (1 * machine['end_info']['switchs']) - (10 * machine['end_info']['switchsInstances']) - (30 * machine['end_info']['unfinesedPriorityCount'])
        
        acumulatorFound += conta

        if (cost != conta):
            print(f"Error = Found cost {cost}, expected {conta}")
            error = True

        if error:
            print("ERROR")
        else:
            print("OK")

    print()
    print(f"Checking Total Cost")


    if (acumulatorFound != planejamento['totalCost']):
        print(f"Error = Found total cost {acumulatorFound}, expected {planejamento['totalCost']}")
    else:
        print("OK")

def checkOperations(machines, jobs):
    print("Checking Operations Done Once")

    acumulator = []

    def tupleLookup(acumulator, job, operation):
        for i, t in enumerate(acumulator):
            if t[0] == job and t[1] == operation:
                return True
        return False

    for i, machine in enumerate(machines):
        print(f"Machine {i+1}/{len(machines)}")
        error = False

        operations = machine['operations']
        
        for operation in operations:
            if tupleLookup(acumulator, operation["job"], operation["operation"]):
                print(f"Error = operation {operation['job']} Operation {operation['operation']} found more than once")
                error = True
            else:
                tupleTeste = (operation["job"], operation["operation"])
                acumulator.append(tupleTeste)
        
        if error:
            print("ERROR")
        else:
            print("OK")        

def checkOrder(machines):
    for i, machine in enumerate(machines):
        print(f"Machine {i+1}/{len(machines)}")
        error = False

        operations = machine['operations']
        machine_info = machine['machine_info']

        for j, operation in enumerate(operations):
            if operation["job"] != machine_info[j]["job"] or operation["operation"] != machine_info[j]["operation"]:
                print(f"Error = Job {operation['job']} Operation {operation['operation']} found in the wrong order")
                error = True
        
        if error:
            print("ERROR")
        else:
            print("OK")

def newKTNS(machines, toolSets, jobs, planejamento):
    print("Running New KTNS")

    totalCost = 0

    TIMESCALE = planejamento['timescale']

    COSTSWITCH         = 1
    COSTSWITCHINSTANCE = 10
    COSTPRIORITY       = 30
    PROFITYFINISHED    = 30

    unsupervised = planejamento['unsupervised']
    planingHorizon = planejamento['planingHorizon']
    
    capacityMagazine = 8

    numberTools = 0
    for machine in machines:
        for operation in machine['machine_info']:
            job = jobLookup(jobs, operation['job'], operation['operation'])
            for tool in toolSets[job['ToolSet']]:
                if tool > numberTools:
                    numberTools = tool
    numberTools += 1
    
    for i, machine in enumerate(machines):
        s = []

        for operation in (machine['machine_info']):
            job, index = jobLookup(jobs, operation['job'], operation['operation'], True)
            s.append(index)

        magazineL = [True] * numberTools
        switchs = 0
        jL = 0

        switchsInstances = 0      # Conta quantas trocas de instancia foram feitas, quando pelo menos uma troca de ferramenta foi trocada do magazine
        currantSwitchs = 0        # Conta quantas trocas de ferramenta foram feitas, no job atual
        currantProcessingTime = 0 

        inicioJob = 0             # Conta quantas horas ja foram usadas no dia atual  
        fimJob = 0                # Conta quantos dias ja foram usados no horizonte de planejamento

        fineshedPriorityCount = 0 
        unfineshedPriorityCount = 0 

        numberJobsSol = len(s) 

        print("MACHINE: ", i)
        
        while jL < numberJobsSol:
            print("JOB: ", jL)

            currantSwitchs = 0 
            magazineCL = [False] * numberTools
            left = jL
            cmL = 0

            while cmL < capacityMagazine and left < numberJobsSol:
                for tool in toolSets[jobs[s[left]]['ToolSet']]:
                    if cmL > capacityMagazine-1: break
                    if magazineL[tool] and not magazineCL[tool]:
                        magazineCL[tool] = True
                        cmL += 1
                    elif jL == left and not magazineCL[tool]:
                        magazineCL[tool] = True
                        cmL += 1
                        currantSwitchs += 1
                    
                left += 1

            t = 0
            while t < numberTools and cmL < capacityMagazine:
                if magazineL[t] and not magazineCL[t]:
                    magazineCL[t] = True
                    cmL += 1
                t += 1

            magazineL = magazineCL 

            if jL == 0:
                currantSwitchs = capacityMagazine 

            currantProcessingTime = jobs[s[jL]]['Processing Time'] 
            fimJob = inicioJob + currantProcessingTime 

            if inicioJob % TIMESCALE >= unsupervised and currantSwitchs > 0: 
                if currantProcessingTime + (inicioJob + (TIMESCALE - (inicioJob % TIMESCALE))) >= planingHorizon * TIMESCALE: 
                    break
                else:
                    inicioJob += TIMESCALE - (inicioJob % TIMESCALE)
                    fimJob = inicioJob + currantProcessingTime

            if (inicioJob % TIMESCALE) + currantProcessingTime >= TIMESCALE:
                if fimJob >= planingHorizon * TIMESCALE:
                    break

            inicioJob = fimJob

            switchs += currantSwitchs
            if currantSwitchs > 0:
                switchsInstances += 1

            fineshedPriorityCount += jobs[s[jL]]['Priority']

            print("(", end="")
            for i, item in enumerate(magazineCL):
                if(item): print(i, end=",")
            print(")", end="")
            print()

            jL += 1


        curCost = (PROFITYFINISHED * fineshedPriorityCount) - (COSTSWITCH * switchs) - (COSTSWITCHINSTANCE * switchsInstances) - (COSTPRIORITY * unfineshedPriorityCount)
        totalCost += curCost

        print(f"fineshedPriorityCount = {fineshedPriorityCount} | switchs = {switchs} | switchsInstances = {switchsInstances} | unfineshedPriorityCount = {unfineshedPriorityCount} | cost = {curCost}")

        if curCost != machine['end_info']['cost']:
            print(f"Error = Found cost {curCost}, expected {machine['end_info']['cost']}")
        else:
            print("OK")
    
    print()
    print("Checking Total Cost")
    if totalCost != planejamento['totalCost']:
        print(f"Error = Found total cost {totalCost}, expected {planejamento['totalCost']}")
    else:
        print("OK")

    return totalCost

# ---------------------------------------------------------------------------------------------------
# MAIN
# ---------------------------------------------------------------------------------------------------

def main():
    option = sys.argv[1]

    if option == 'single':
        report = sys.argv[2]

        print(f"---Validating {report}---")

        machines, planejamento = rp.parseReport(report)
        toolSets = ld.loadToolSet(planejamento['toolSetFileName'])
        jobs = ld.loadJobs(planejamento['jobsFileName'])

        # checkMagazine(machines, toolSets, jobs)
        # print()
        # checkUnsupervisedSwitchs(machines, toolSets, jobs, planejamento)
        # print()
        # checkSwitchs(machines, toolSets, jobs)
        # print()
        # checkUnfinishedJobs(machines, jobs)
        # print()
        # checkOperations(machines, jobs)
        # print()
        # checkOrder(machines)
        # print()
        # checkProfit(machines, jobs, planejamento)
        # print()
        newKTNS(machines, toolSets, jobs, planejamento)
        print()
    
    if option == 'multiple':
        batchFile = sys.argv[2]
        file = open(batchFile, 'r')

        for i, line in enumerate(file):
            report = line.strip()

            print(f"---Validating {report}---")

            machines, planejamento = rp.parseReport(report)
            toolSets = ld.loadToolSet(planejamento['toolSetFileName'])
            jobs = ld.loadJobs(planejamento['jobsFileName'])

            # checkMagazine(machines, toolSets, jobs)
            # print()
            # checkUnsupervisedSwitchs(machines, toolSets, jobs, planejamento)
            # print()
            # checkSwitchs(machines, toolSets, jobs)
            # print()
            # checkUnfinishedJobs(machines, jobs)
            # print()
            # checkOperations(machines, jobs)
            # print()
            # checkOrder(machines)
            # print()
            # checkProfit(machines, jobs, planejamento)
            # print()
            newKTNS(machines, toolSets, jobs, planejamento)
            print()

main()	


# obj = rp.parseReport("/home/mateus/WSL/IC/SSP/output/exemploArtigo.txt")
# print(obj)