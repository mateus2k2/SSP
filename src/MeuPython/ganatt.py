import matplotlib.pyplot as plt
import mpld3
from mpld3 import plugins
import numpy as np
from bokeh.plotting import figure, show
from bokeh.models import Span

def plot_machine(machines, planejamento):
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
            
            corAtual = colors['priority'] if operation['priority'] == 1 else colors['normal']
            
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

def plot_machine_bokeh(machines, planejamento):
    planingHorizon = planejamento['planingHorizon']
    unsupervised = planejamento['unsupervised']
    timescale = planejamento['timescale']
    numberMachines = len(machines)

    p = figure(y_range=(-1, numberMachines), plot_height=400, plot_width=800, x_axis_label='Time (hours)', y_axis_label='Machines')

    colors = {
        'priority': 'blue',
        'normal': 'orange',
    }

    for i, machine in enumerate(machines):
        operations = machine['operations']
        machine_info = machine['machine_info']
        end_info = machine['end_info']

        for j, operation in enumerate(operations):
            corAtual = colors['priority'] if operation['priority'] == 1 else colors['normal']

            barY = (numberMachines - i - (i % 2))
            barWidth = operation['end'] - operation['start']

            p.hbar(y=barY, left=operation['start'], right=operation['end'], height=0.3, line_color='black', fill_color=corAtual, legend_label=operation['job'])

            fontsize = '8pt'

            job_operation_text_x = operation['start'] + barWidth / 2
            p.text(x=job_operation_text_x, y=barY, text=f"({operation['job']}, {operation['operation']})", text_color='white', text_align='center', text_baseline='middle', text_font_size=fontsize)

    vertical_lines = []
    x_ticks = []
    count = 0
    for i in range(0, planingHorizon + 1):
        vertical_lines.append(count)
        vertical_lines.append(count + unsupervised)
        x_ticks.append(count)
        x_ticks.append(count + unsupervised)
        count += timescale

    vertical_lines.pop()

    for line_position in vertical_lines:
        vline = Span(location=line_position, dimension='height', line_color='gray', line_dash='dotted', line_width=1)
        p.add_layout(vline)

    for i in range(0, len(vertical_lines) - 1, 2):
        vspan = p.varea(x=[vertical_lines[i], vertical_lines[i + 1]], y1=numberMachines, y2=-1, fill_color='lightgray', fill_alpha=0.5, legend_label='Highlighted Area')
        p.add_layout(vspan)

    p.yaxis.ticker = range(-1, len(machines))
    p.yaxis.major_label_overrides = {i: str(i) for i in range(-1, len(machines))}

    p.xaxis.ticker = x_ticks
    p.xaxis.major_label_overrides = {tick: str(tick) for tick in x_ticks}

    show(p)

def parse_machine_section(machine_section):
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
            'maganize': list(map(int, operation[5][:-1].split(',')))
        })
    
    endInfoObj['fineshedPriorityCount'] = int(end_info[1])
    endInfoObj['switchs'] = int(end_info[2])
    endInfoObj['switchsInstances'] = int(end_info[3])
    endInfoObj['unfinesedPriorityCount'] = int(end_info[4])
    endInfoObj['cost'] = int(end_info[5])
        
    return machineInfoObj, operationsObj, endInfoObj

def parse_file(file_path):
    file = open(file_path, 'r')
    file_content = file.read()
    
    lines = file_content.splitlines()
    header = lines.pop(0)
    planejamento = [int(num) for num in header.split(';')]
    planejamentoObj = {
        'planingHorizon': planejamento[0],
        'unsupervised': planejamento[1],
        'timescale': planejamento[2]
        
    }
    
    modified_string = '\n'.join(lines)
    
    machines = []
    machine_sections = modified_string.strip().split('Machine:')
    for machine_section in machine_sections[1:]:
        machine_info, operations, end_info = parse_machine_section(machine_section)
        machines.append({
            'machine_info': machine_info,
            'operations': operations,
            'end_info': end_info
        })
    return machines, planejamentoObj

def printReport(machines, planejamento):
    print(f"planingHorizon = {planejamento['planingHorizon']}")
    print(f"unsupervised = {planejamento['unsupervised']}")
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
            print(f"maganize = {operation['maganize']}") 
            print()      
        
        print(f"end_info = {end_info}")
        print("\n----------------------------------------------------------------\n")

machines, planejamento = parse_file('/home/mateus/WSL/IC/data/solutionReport.txt')
# plot_machine(machines, planejamento)
plot_machine_bokeh(machines, planejamento)
# printReport(machines, planejamento)
