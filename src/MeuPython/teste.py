from bokeh.plotting import figure, output_file, save
from bokeh.models import Label, DataRange1d, CustomJS
from bokeh.layouts import column, row
from bokeh.plotting import figure, curdoc

PLOT_INIT_X_MAX = 10

# set output to HTML file
output_file(filename="text_scale.html")

# create a new plot with specific size and tools
p = figure(width=500, height=300, tools="pan,wheel_zoom,box_zoom,reset", active_scroll ="wheel_zoom")

# set the initial x range
p.x_range = DataRange1d(start=0, end=PLOT_INIT_X_MAX)

# add sample circle plot
p.circle([1,2,3,4,5,6,7,8,9,10], [1,2,3,4,5,6,7,8,9,10], size=5)

# add sample text
sample_text = Label(x=5, y=5, text='sample text')
p.add_layout(sample_text)

# handle dynamic text rescaling with CustomJS
# cb_obj.start and cb_obj.end are the start and end values of x axis
x_callback = CustomJS(args=dict(sample_text=sample_text, init_font_size=sample_text.text_font_size[:-2], init_xrange=PLOT_INIT_X_MAX), code="""
let xzoom = (init_font_size * init_xrange) / (cb_obj.end - cb_obj.start);
sample_text['text_font_size'] = String(xzoom) + 'pt';
""")
p.x_range.js_on_change('start', x_callback)

# save the results to HTML file
# save(p)
layout = row(p)
curdoc().add_root(layout)


# bokeh serve --show src/MeuPython/teste.py