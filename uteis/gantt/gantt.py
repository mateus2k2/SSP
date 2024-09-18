import matplotlib.pyplot as plt
import numpy as np

def plotMachines(machines, planejamento):
    planingHorizon = planejamento['planingHorizon']
    unsupervised = planejamento['unsupervised']
    timescale = planejamento['timescale']
    numberMachines = len(machines)
    
    fig, ax = plt.subplots()

    colors = {
        'priority': 'tab:blue',
        'normal': 'tab:orange',
    }
    
    for i, machine in enumerate(machines):
        operations = machine['operations']
        machine_info = machine['machine_info']
        end_info = machine['end_info']
        
        for j, operation in enumerate(operations):
            
            corAtual = colors['normal']
            # corAtual = colors['priority'] if operation['priority'] == 1 else colors['normal']
            
            barY = (numberMachines-i-(i%2))
            barWidth = operation['end'] - operation['start']           
            ax.barh(barY, barWidth, left=operation['start'], align='center', edgecolor='black', height=0.3, color=corAtual, label=operation['job'])
            
            fontsize = 8
            
            jobOperationTextX = operation['start'] + barWidth / 2
            ax.text(jobOperationTextX, barY, f"({operation['job']}, {operation['operation']})", color='white', ha='center', va='center', fontsize=fontsize)
            
            # magazineTextX = operation['start'] + barWidth / 2
            # magazineTextY = (barY - 0.5)
            # ax.text(magazineTextX, magazineTextY, f"{operation['maganize']}", color='black', ha='center', va='center', fontsize=fontsize)

    verticalLines = []
    xticks = []
    count = 0
    for i in range(0, planingHorizon+1):
        verticalLines.append(count)
        verticalLines.append(count+unsupervised)
        xticks.append(count)
        xticks.append(count+unsupervised)
        count += timescale
    
    verticalLines.pop()
    
    for i, linePosition in enumerate(verticalLines):
        ax.axvline(linePosition, color='gray', linestyle='dotted', linewidth=1)
        if i < len(verticalLines)-1 and i%2 == 1: 
            ax.axvspan(verticalLines[i], verticalLines[i+1], facecolor='lightgray', alpha=0.5, label='Highlighted Area')

    ax.set_yticks(range(-1, len(machines)))
    ax.set_yticklabels(range(-1, len(machines)))
    
    ax.set_xticks(xticks)
    ax.set_xticklabels(xticks)
    
    ax.set_xlabel('Time (hours)')
    ax.set_ylabel('Machines')
    
    
    plt.show()

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

# machines, planejamento = parseReport('/home/mateus/WSL/IC/SSP/output/exemploArtigoRuim.txt')
# plotMachines(machines, planejamento)