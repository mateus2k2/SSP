from bokeh.plotting import figure, curdoc
from bokeh.models import ColumnDataSource, HoverTool, TapTool, CustomJS
from bokeh.layouts import column
from bokeh.models.widgets import Div

# Create a ColumnDataSource with some data
data = {'x': [1, 2, 3, 4, 5],
        'y': [6, 7, 2, 4, 8],
        'text': ['Point 1', 'Point 2', 'Point 3', 'Point 4', 'Point 5']}
source = ColumnDataSource(data=data)

# Create a figure
p = figure(title='Click a point to show/hide text', tools='tap')

# Plot the points
p.circle('x', 'y', size=10, source=source)

# Add hover tool
hover = HoverTool(tooltips=[("Index", "@index"), ("(x, y)", "($x, $y)"), ("Text", "@text")])
p.add_tools(hover)

# Add a Div to display text
text_div = Div(text="", width=400)
layout = column(p, text_div)

# Create a CustomJS callback to update the text_div when a point is clicked
callback = CustomJS(args=dict(source=source, text_div=text_div), code="""
    var selected_index = cb_obj.indices[0];
    var text = source.data['text'][selected_index];
    text_div.text = "<h2>" + text + "</h2>";
""")

# Add TapTool with the callback to the figure
tap_tool = TapTool(callback=callback)
p.add_tools(tap_tool)

# Set up the document
curdoc().add_root(layout)
