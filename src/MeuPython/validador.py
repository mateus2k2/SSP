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

machines, planejamento = parseReport('/home/mateus/WSL/IC/data/SolutionReports/solutionReport.txt')
toolSet, indexList = ld.loadToolSet(planejamento['toolSetFileName'], returnIndex=True)
jobs = ld.loadJobs(planejamento['jobsFileName'])

print(len(jobs))
print(len(toolSet))
print(len(indexList))

# Validar se nao ha trocas em periodos nao supervisionados = Fazer a contagem do tempo independete 
# Validar se a magazine contem as ferramentas para realizar o dado trabalho
# Validar se a sequencia de tarefas resulta no numero de trocas esperado = Tanto em termos de numero de vezes q foram trocadas quanto no numero de trocas em si = PREJUIZO 
# Validar se a sequencia de tarefas resulta no numero de tarefas prioritarias nao terminadas esperado = PREJUIZO
# Validar se a sequencia de tarefas resulta no numero de tarefas prioritarias nao terminadas esperado = LUCRO
# Fazer a conta do prejuizo e do lucro e validar com a recebida