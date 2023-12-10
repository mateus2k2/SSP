# from bokeh.plotting import figure, curdoc
# from bokeh.models import ColumnDataSource, HoverTool, TapTool, CustomJS
# from bokeh.layouts import column
# from bokeh.models.widgets import Div

# # Create a ColumnDataSource with some data
# data = {'x': [1, 2, 3, 4, 5],
#         'y': [6, 7, 2, 4, 8],
#         'text': ['Point 1', 'Point 2', 'Point 3', 'Point 4', 'Point 5']}
# source = ColumnDataSource(data=data)

# # Create a figure
# p = figure(title='Click a point to show/hide text', tools='tap')

# # Plot the points
# p.circle('x', 'y', size=10, source=source)

# # Add hover tool
# hover = HoverTool(tooltips=[("Index", "@index"), ("(x, y)", "($x, $y)"), ("Text", "@text")])
# p.add_tools(hover)

# # Add a Div to display text
# text_div = Div(text="", width=400)
# layout = column(p, text_div)

# # Create a CustomJS callback to update the text_div when a point is clicked
# callback = CustomJS(args=dict(source=source, text_div=text_div), code="""
#     var selected_index = cb_obj.indices[0];
#     var text = source.data['text'][selected_index];
#     text_div.text = "<h2>" + text + "</h2>";
# """)

# # Add TapTool with the callback to the figure
# tap_tool = TapTool(callback=callback)
# p.add_tools(tap_tool)

# curdoc().add_root(layout)







# from bokeh.io import curdoc
# from bokeh.layouts import column
# from bokeh.models import Button, Div

# # Initial content
# initial_content = """
# <h3>Click the button to expand and collapse</h3>
# <p>This is some information that can be expanded or collapsed.</p>
# """

# # Create a Div widget to display content
# info_div = Div(text=initial_content, width=400)

# # Create a Button widget for expand/collapse action
# expand_collapse_button = Button(label="Expand/Collapse", button_type="success")

# # Callback function for the button click event
# def expand_collapse():
#     if info_div.visible:
#         info_div.visible = False
#         expand_collapse_button.label = "Expand"
#     else:
#         info_div.visible = True
#         expand_collapse_button.label = "Collapse"

# # Attach the callback function to the button click event
# expand_collapse_button.on_click(expand_collapse)

# # Set up the layout
# layout = column(expand_collapse_button, info_div)

# # Add the layout to the current document
# curdoc().add_root(layout)




# from bokeh.models import ColumnDataSource, Div
# from bokeh.plotting import figure, curdoc
# from bokeh.layouts import column
# from bokeh.events import Tap

# # Sample data
# data = {'x': [1, 2, 3, 4, 5],
#         'y': [6, 7, 2, 4, 8],
#         'info': ['Point 1', 'Point 2', 'Point 3', 'Point 4', 'Point 5']}

# # Create a ColumnDataSource
# source = ColumnDataSource(data=data)

# # Create a figure
# plot = figure(width=400, height=400, tools='tap', title='Click on a point')

# # Plot the points
# plot.circle('x', 'y', size=10, source=source)

# # Create a Div to display information
# info_div = Div(width=300, height=400, text='Click on a point to see information')

# # Callback function to update the info_div when a point is clicked
# def callback(event):
#     selected_index = source.selected.indices
#     if selected_index:
#         info_text = f"Information about {data['info'][selected_index[0]]}"
#     else:
#         info_text = "Click on a point to see information"
#     info_div.text = info_text

# # Attach the callback function to the Tap event
# plot.on_event(Tap, callback)

# # Create a layout with the plot and info_div
# layout = column(plot, info_div)

# # Add the layout to the current document
# curdoc().add_root(layout)








import matplotlib.pyplot as plt
import mpld3
from mpld3 import plugins
import numpy as np
from bokeh.plotting import figure, show
from bokeh.models import Span
from bokeh.models import ColumnDataSource, Div, Text, VArea
from bokeh.plotting import figure, curdoc
from bokeh.layouts import column
from bokeh.events import Tap

def plot_machine_bokeh(machines, planejamento):
    planingHorizon = planejamento['planingHorizon']
    unsupervised = planejamento['unsupervised']
    timescale = planejamento['timescale']
    numberMachines = len(machines)

    p = figure(y_range=(-1, numberMachines), height=400, width=800, x_axis_label='Time (hours)', y_axis_label='Machines')

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

            p.hbar(y=barY, left=operation['start'], right=operation['end'], height=0.3, line_color='black', fill_color=corAtual, legend_label=f"operation['job']")

            job_operation_text_x = operation['start'] + barWidth / 2
            source = ColumnDataSource(data=dict(x=[job_operation_text_x], y=[barY], text=[f"({operation['job']}, {operation['operation']})"]))
            glyph = Text(x="x", y="y", text="text", angle=0, text_color="#96deb3")
            p.add_glyph(source, glyph)
            
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
        N = 30
        x = np.linspace(-2, 3, N)
        y1 = np.zeros(N)
        y2 = 10 - x**2
        
        source = ColumnDataSource(dict(x=x, y1=y1, y2=y2))
        glyph = VArea(x="x", y1="y1", y2="y2", fill_color="#f46d43")
        p.add_glyph(source, glyph)
        
    # p.yaxis.ticker = [i in range(-1, len(machines))]
    # p.yaxis.major_label_overrides = {i: str(i) for i in range(-1, len(machines))}

    # p.xaxis.ticker = x_ticks
    # p.xaxis.major_label_overrides = {tick: str(tick) for tick in x_ticks}

    layout = column(p)
    curdoc().add_root(layout)


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
