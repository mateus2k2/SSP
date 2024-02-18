import loadData as ld

def parseMachineSection(machine_section):
    lines = machine_section.strip().split('\n')
    machine_info = lines[0].split('=')[1].strip()
    operations = [line.split(';') for line in lines[1:-1]]
    end_info = lines[-1].split(';')
    
    operationsObj = []
    endInfoObj = {}
    machineInfoObj = []
    
    pairs = machine_info[:-1].split(";")
    job_operation_list = [tuple(map(int, pair.strip('()').split(','))) for pair in pairs]
    for job_operation in job_operation_list:
        machineInfoObj.append({
            'job': int(job_operation[0]),
            'operation': int(job_operation[1])
        })

    for operation in operations:
        operationsObj.append({
            'job': int(operation[0]),
            'operation': int(operation[1]),
            'start': int(operation[2]),
            'end': int(operation[3]),
            'priority': int(operation[4]),
            'magazine': list(map(int, operation[5][:-1].split(',')))
        })
    
    endInfoObj['fineshedPriorityCount'] = int(end_info[1])
    endInfoObj['switchs'] = int(end_info[2])
    endInfoObj['switchsInstances'] = int(end_info[3])
    endInfoObj['unfinesedPriorityCount'] = int(end_info[4])
    endInfoObj['cost'] = int(end_info[5])
        
    return machineInfoObj, operationsObj, endInfoObj

def parseReport(file_path):
    file = open(file_path, 'r')
    file_content = file.read()
    
    lines = file_content.splitlines()
    
    instancesName = lines.pop(0)
    jobsFileName = instancesName.split(';')[0]
    toolSetFileName = instancesName.split(';')[1]
    
    header = lines.pop(0)
    planejamento = [int(num) for num in header.split(';')]
    planejamentoObj = {
        'planingHorizon': planejamento[0],
        'unsupervised': planejamento[1],
        'timescale': planejamento[2],
        'jobsFileName': jobsFileName,
        'toolSetFileName': toolSetFileName
    }
    
    modified_string = '\n'.join(lines)
    
    machines = []
    machine_sections = modified_string.strip().split('Machine:')
    for machine_section in machine_sections[1:]:
        machine_info, operations, end_info = parseMachineSection(machine_section)
        machines.append({
            'machine_info': machine_info,
            'operations': operations,
            'end_info': end_info
        })
    return machines, planejamentoObj

def printReport(machines, planejamento):
    print(f"planingHorizon = {planejamento['planingHorizon']}")
    print(f"unsupervised = {planejamento['unsupervised']}")
    print(f"timescale = {planejamento['timescale']}")
    print(f"instanceName = {planejamento['instanceName']}")
    
    print("\n----------------------------------------------------------------\n")
    
    for machine in machines:
        operations = machine['operations']
        machine_info = machine['machine_info']
        end_info = machine['end_info']
        
        print(f"machine_info = {machine_info}")
        print()
        
        for operation in operations:
            print(f"job = {operation['job']} operation {operation['operation']}")
            print(f"start = {operation['start']}")
            print(f"end = {operation['end']}")
            print(f"priority = {operation['priority']}")
            print(f"magazine = {operation['magazine']}")
            print()      
        
        print(f"end_info = {end_info}")
        print("\n----------------------------------------------------------------\n")

def jobLookup(jobs, job, operation):
    for j in jobs:
        if j['Job'] == job and j['Operation'] == operation:
            return j
    return None

def checkMagazine (machine, toolSets, jobs):
    for i, machine in enumerate(machines):
        print(f"Machine {i+1}/{len(machines)}")
        for j, operation in enumerate(machine['operations']):
            realJob = jobLookup(jobs, operation['job'], operation['operation'])

            if realJob == None:
                print(f"{i+1}/{len(machine['operations'])} | Job {operation['job']} Operation {operation['operation']} not found")
            
            if not set(toolSets[realJob['ToolSet']]).issubset(set(operation['magazine'])):
                print(f"{i+1}/{len(machine['operations'])} | Job {operation['job']} Operation {operation['operation']} ToolSet {realJob['ToolSet']} not found in magazine")
                print(f"Magazine: {operation['magazine']}")
                print(f"ToolSet: {toolSets[realJob['ToolSet']]}")
                print(f"Diference: {set(toolSets[realJob['ToolSet']]) - set(operation['magazine'])}")
                print()

def checkUnsupervisedSwitchs(machines, toolSets, jobs, planejamento):
    unsupervisedStart = planejamento['unsupervised']
    timeScale = planejamento['timescale']

    for i, machine in enumerate(machines):
        curTime = 0

        for j, operation in enumerate(machine['operations']):
            curJob = jobLookup(jobs, operation['job'], operation['operation'])
            curToolSet = toolSets[curJob['ToolSet']]

            lastJob = jobLookup(jobs, machine['operations'][j-1]['job'], machine['operations'][j-1]['operation'])
            lastToolSet = toolSets[lastJob['ToolSet']]

            curTime = operation['start']

            # if (curTime + expectedTime) != operation['end'] or (expectedTime) != curTime:
            #     print("TEMPO ERRADO")
            #     print(f"expectedTime = {expectedTime}")
            #     print(f"curTime = {curTime}")
            #     print()                    

            if curTime%timeScale >= unsupervisedStart:
                if operation['magazine'] != machine['operations'][j-1]['magazine'] and len(set(curToolSet) - set(lastToolSet)) > 0:
                    print(f"Unsupervised Switch in Machine {i+1}/{len(machines)}")
                    print(f"Job {operation['job']} Operation {operation['operation']}")
                    print(f"Magazine: {operation['magazine']}")
                    print(f"Previous Magazine: {machine['operations'][j-1]['magazine']}")
                    print(f"curentTime = {curTime} | timeScale = {timeScale} | unsupervisedStart = {unsupervisedStart} | curTime%timeScale = {curTime%timeScale}")
                    print()
        #     break
        # break

def checkSwitchs(machines, toolSets, jobs):

    for i, machine in enumerate(machines):
        switchInstance = 0
        toolSwitchs = 0

        for j, operation in enumerate(machine['operations']):
            realJob = jobLookup(jobs, operation['job'], operation['operation'])
            realToolSet = toolSets[realJob['ToolSet']]

            if j == 0:
                switchInstance += 1
                toolSwitchs += len(operation['magazine'])
            else:
                if operation['magazine'] != machine['operations'][j-1]['magazine']:
                    switchInstance += 1
                    toolSwitchs += len(set(operation['magazine']) - set(machine['operations'][j-1]['magazine']))
        
        print(f"switchInstance = {switchInstance} | toolSwitchs = {toolSwitchs}")
        print(f"end_info = {machine['end_info']}")
        print()

def checkUnfinishedJobs(machines, jobs):
    for i, machine in enumerate(machines):
        numberOfPriorityJobsExpected = 0
        numerOfFinishedPriorityJobs = 0

        for k, job in enumerate(jobs):
            if job['Priority'] == 1: numberOfPriorityJobsExpected += 1

        for j, operation in enumerate(machine['operations']):
            realJob = jobLookup(jobs, operation['job'], operation['operation'])
            if realJob['Priority'] == 1 and operation['priority'] == 1: numerOfFinishedPriorityJobs += 1            

        print(f"numberOfPriorityJobsExpected = {numberOfPriorityJobsExpected} | numerOfFinishedPriorityJobs = {numerOfFinishedPriorityJobs}")
        print(f"end_info = {machine['end_info']}")
        print()

# endInfoObj['fineshedPriorityCount'] = int(end_info[1])
# endInfoObj['switchs'] = int(end_info[2])
# endInfoObj['switchsInstances'] = int(end_info[3])
# endInfoObj['unfinesedPriorityCount'] = int(end_info[4])
# endInfoObj['cost'] = int(end_info[5])

# int cost = (PROFITYPRIORITY * fineshedPriorityCount) - (COSTSWITCH * switchs) - (COSTSWITCHINSTANCE * switchsInstances) - (COSTPRIORITY * unfineshedPriorityCount);

def checkProfit(machines, jobs):
    for i, machine in enumerate(machines):
        cost = machine['end_info']['cost']
        conta = (30 * machine['end_info']['fineshedPriorityCount']) - (1 * machine['end_info']['switchs']) - (10 * machine['end_info']['switchsInstances']) - (30 * machine['end_info']['unfinesedPriorityCount'])
        
        print(f"cost = {cost}")
        print(f"conta = {conta}")
        print()

machines, planejamento = parseReport('/home/mateus/WSL/IC/data/SolutionReports/solutionReport.txt')
toolSets = ld.loadToolSet(planejamento['toolSetFileName'])
jobs = ld.loadJobs(planejamento['jobsFileName'])

# checkMagazine(machines, toolSets, jobs)
# checkUnsupervisedSwitchs(machines, toolSets, jobs, planejamento)
# checkSwitchs(machines, toolSets, jobs)
# checkUnfinishedJobs(machines, jobs)
# checkProfit(machines, jobs)


# OK Validar se a magazine contem as ferramentas para realizar o dado trabalho
# OK Validar se nao ha trocas em periodos nao supervisionados = Fazer a contagem do tempo independete 
# OK Validar se a sequencia de tarefas resulta no numero de trocas esperado = Tanto em termos de numero de vezes q foram trocadas quanto no numero de trocas em si = PREJUIZO 
# OK Validar se a sequencia de tarefas resulta no numero de tarefas prioritarias nao terminadas esperado = PREJUIZO
# OK Validar se a sequencia de tarefas resulta no numero de tarefas prioritarias terminadas esperado = LUCRO
# OK Fazer a conta do prejuizo e do lucro e validar com a recebida