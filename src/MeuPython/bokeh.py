import matplotlib.pyplot as plt
import mpld3
from mpld3 import plugins
import numpy as np
from bokeh.plotting import figure, show
from bokeh.models import Span
from bokeh.models import ColumnDataSource, Div, Text, VArea, Patch, Button, Label
from bokeh.plotting import figure, curdoc, save
from bokeh.layouts import column, row
from bokeh.events import Tap
from bokeh.models.callbacks import CustomJS

def plot_machine_bokeh(machines, planejamento):
    planingHorizon = planejamento['planingHorizon']
    unsupervised = planejamento['unsupervised']
    timescale = planejamento['timescale']
    numberMachines = len(machines)

    p = figure(y_range=(-1, numberMachines+4), height=400, width=800, x_axis_label='Time (hours)', y_axis_label='Machines')

    machines_source = ColumnDataSource(data={
        'machines': machines,
    })
    
    colors = {
        'priority': 'blue',
        'normal': 'orange',
    }
    
    # task_info_list = Div(width=400, height=200, text="<ol></ol>")

    # button_callback = CustomJS(args=dict(task_info_list=task_info_list), code="""
    #     // Remove the clicked list item when the button is pressed
    #     var selected_li = document.querySelector('.selected');
    #     selected_li.parentNode.removeChild(selected_li);
    # """)

    # tap_callback = CustomJS(args=dict(task_info_list=task_info_list, machines_source=machines_source, button_callback=button_callback), code="""
    #     var x_click = cb_obj.x;
    #     var y_click = cb_obj.y;
    #     var machines_data = machines_source.data;
    #     var task_info = "Task Information: ...";  // Update this line with the actual task information
    #     task_info_list.text += "<li class='selected'>" + task_info + "<button onclick='button_callback.execute()'>Close</button></li>";
    # """)
    

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
        overlay_x = [vertical_lines[i], vertical_lines[i + 1], vertical_lines[i + 1], vertical_lines[i]]
        overlay_y = [-1, -1, numberMachines, numberMachines]
        p.patch(overlay_x, overlay_y, color='gray', alpha=0.3)
        
    # p.yaxis.ticker = [i in range(-1, len(machines))]
    # p.yaxis.major_label_overrides = {i: str(i) for i in range(-1, len(machines))}

    p.xaxis.ticker = x_ticks
    p.xaxis.major_label_overrides = {tick: str(tick) for tick in x_ticks}


    textsList = []
    
    for i, machine in enumerate(machines):
        operations = machine['operations']
        machine_info = machine['machine_info']
        end_info = machine['end_info']

        for j, operation in enumerate(operations):
            corAtual = colors['priority'] if operation['priority'] == 1 else colors['normal']

            barY = (numberMachines - i - (i % 2))
            barWidth = operation['end'] - operation['start']

            p.hbar(y=barY, left=operation['start'], right=operation['end'], height=0.3, line_color='black', fill_color="blue", legend_label=f"operation['job']", alpha=0.3)

            label_x = operation['start'] + barWidth / 2
            label_y = barY
            label_text = f"({operation['job']}, {operation['operation']})"
            
            text = Label(x=label_x, y=label_y, text=label_text, text_align='center', text_baseline='middle', text_color='black', text_font_size='10pt')
            textsList.append(text)
            p.add_layout(text)
            


    x_callback = CustomJS(args=dict(textsList=textsList, init_font_size=1, init_xrange=9360), code="""
    let xzoom = ((init_font_size * init_xrange) / (cb_obj.end - cb_obj.start));
    
    for (let i = 0; i < textsList.length; i++) {
        textsList[i]['text_font_size'] = xzoom + 'pt';
    }
    """)
    p.x_range.js_on_change('start', x_callback)      
            
    p.grid.grid_line_color = None
    p.ygrid.grid_line_color = None
    
    # p.js_on_event('tap', tap_callback)
    # layout = row(p, column(task_info_list))

    layout = row(p, column())
    curdoc().add_root(layout)
    save(layout, filename='/home/mateus/WSL/IC/data/bokeh.html')


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
