import re

# ---------------------------------------------------------------------------------------------------
# REPORT PARSER
# ---------------------------------------------------------------------------------------------------

def parseMachineSection(machine_section):
    lines = machine_section.strip().split('\n')
    operations = [line.split(';') for line in lines[1:]]
    
    operationsObj = []
    
    for operation in operations:
        operationsObj.append({
            'job': int(operation[0]),
            'operation': int(operation[1]),
            'start': int(operation[2]),
            'end': int(operation[3]),
            'priority': int(operation[4]),
            'magazine': list(map(int, operation[5][:-1].split(',')))
        })

    return operationsObj

def to_camel_case(s):
    words = re.split(r'[\s_-]+', s)
    return words[0].lower() + ''.join(word.capitalize() for word in words[1:])

def parseReport(file_path):
    file = open(file_path, 'r')
    file_content = file.read()
    
    linesAux = file_content.splitlines()
    end_index = linesAux.index("END")

    lines = linesAux[:end_index]
    end_info = linesAux[end_index + 1:]
    
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
        'toolSetFileName': toolSetFileName,
    }
    
    modified_string = '\n'.join(lines)
    
    machines = []
    machine_sections = modified_string.strip().split('Machine:')
    for machine_section in machine_sections[1:]:
        machine_info = parseMachineSection(machine_section)
        machines.append(machine_info)
    
    endInfoObj = {}
    for item in end_info:
        key, value = item.replace(';', '').split(':') if ':' in item else item.split(' ', 1)
        endInfoObj[to_camel_case(key.strip())] = float(value.strip())
    
    return  planejamentoObj, machines, endInfoObj

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

# parseReport("/home/mateus/WSL/IC/SSP/output/Exemplo/exemplo.txt")
