import matplotlib.pyplot as plt
plt.rcParams['svg.fonttype'] = 'path'

# Sample data
tasks = {
    "Task 1": [(0, 5)],     # Task 1 runs from hour 0 to hour 5
    "Task 2": [(5, 7)],     # Task 2 runs from hour 2 to hour 7
    "Task 3": [(7, 10)],    # Task 3 runs from hour 6 to hour 10
    # Add more tasks as needed
}

# Set up the plot
fig, ax = plt.subplots()

# Define colors for each task
colors = ['red', 'green', 'blue']

# Plot each task as a horizontal bar with specified color
for i, (task, time_intervals) in enumerate(tasks.items()):
    for start, end in time_intervals:
        bar_width = end - start
        ax.barh(2, bar_width, left=start, align='center', edgecolor='black', height=0.6, color='red', label=task)
        
        # Add text inside the bar
        text_x = start + bar_width / 2
        text_y = 2
        ax.text(text_x, text_y, f"{task}\n{start}-{end}", color='white', ha='center', va='center')
        
        # Add text inside the bar
        text_x = start + bar_width / 2
        text_y = 1
        ax.text(text_x, text_y, f"texto embaixo", color='black', ha='center', va='center')

# Add dotted vertical lines at specific positions
vertical_lines = [3, 8]
for line_position in vertical_lines:
    ax.axvline(line_position, color='gray', linestyle='dotted', linewidth=1)


# Customize the plot
ax.set_yticks(range(len(tasks)))
ax.set_yticklabels(list(tasks.keys()))
ax.set_xlabel('Time (hours)')
ax.set_ylabel('Tasks')
ax.legend()

# Show the plot
plt.show()
