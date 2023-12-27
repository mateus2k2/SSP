let parsedData = null;
const fileInput = document.getElementById('fileInput');
fileInput.addEventListener('change', handleFileSelect);

function createGanttChart(data) {
    const ganttChart = document.getElementById('gantt-chart');
    ganttChart.innerHTML = '';

    for (let i = 0; i < data["machines"].length; i++) {
        const machine = document.createElement('div');
        machine.className = 'chart-machine';

        data["machines"][i]["operations"].forEach(job => {
            const block = document.createElement('div');
            block.className = 'chart-block';
            block.style.width = `${calculateWidth(job)}px`;
            block.style.left = `${calculateLeftPosition(job)}px`;
            block.addEventListener('click', () => showDetails(job));
        
            machine.appendChild(block);
        });

        const space = document.createElement('div');
        machine.appendChild(space);


        ganttChart.appendChild(machine);

    }
}

function calculateWidth(job) {
    const startTime = parseInt(job["start"]);
    const endTime = parseInt(job["end"]);

    return ((endTime - startTime) / (24 * 60)) * 1000;
}

function calculateLeftPosition(job) {
    const startTime = parseInt(job["start"]);
    const endTime = parseInt(job["end"]);

    return ((startTime) / (24 * 60)) * 1000;
}

function showDetails(job) {
    const detailsContainer = document.createElement('div');
    detailsContainer.className = 'chart-details';
    detailsContainer.innerHTML = `
    <p><strong>Job Details:</strong></p>
    <p><strong>Index:</strong> ${job[0]}</p>
    <p><strong>Operation Index:</strong> ${job[1]}</p>
    <p><strong>Start Time:</strong> ${job[2]}</p>
    <p><strong>End Time:</strong> ${job[3]}</p>
    <p><strong>Priority:</strong> ${job[4]}</p>
    <p><strong>Machine Magnitude:</strong> ${job[5]}</p>
    `;

    document.body.appendChild(detailsContainer);

    detailsContainer.addEventListener('click', () => {
    detailsContainer.style.display = 'none';
    });
}


function handleFileSelect(event) {
    const file = event.target.files[0];

    if (file) {
        const reader = new FileReader();
        reader.onload = function (e) {
            const fileContent = e.target.result;
            parsedData = parseFileFunc(fileContent);
            console.log(parsedData);
            createGanttChart(parsedData);
        };
        reader.readAsText(file);
    }
}