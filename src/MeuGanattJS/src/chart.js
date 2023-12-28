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
            block.style.margin = `${calculateLeftPosition(job)}px`;
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





function Timeline(data){
        const margin = ({ top: 10, right: 20, bottom: 50, left: 20 }); 
  
        const barHeight = 20;

        const labelStep = 720; //---------------------------------------
      
        const maxYear = data['planejamentoObj']["planingHorizon"] * data['planejamentoObj']['timescale'];
        const minYear = 0
      
        const width = maxYear + margin.left + margin.right;
        
        const yPos = Array.from({ length: 100 }, () => 0); //---------------------------------------
     
        const yPosMax = Math.max(...yPos);
        const yPosMin = Math.min(...yPos);
        const chartHeight = (yPosMax - yPosMin) * barHeight * 2;
        const height = chartHeight + margin.top + margin.bottom;

        const xScale = d3.scaleLinear().domain([minYear, maxYear]).range([margin.left, width - margin.right]);
        const yScale = d3.scalePoint().domain(d3.range(yPosMin, yPosMax + 1)).range([height - margin.bottom, margin.top]).padding(1.5);
      
        const svg = d3.select(document.createElementNS("http://www.w3.org/2000/svg", "svg"))
          .attr("width", width)
          .attr("height", height);
      
        const centuries = d3.range(0, maxYear, data['planejamentoObj']['unsupervised']); //---------------------------------------

        const linesLayer = svg.append("g").attr("class", "lines-layer"); 
      
        linesLayer.append("g")
          .selectAll("line")
          .data(centuries)
          .join("line")
            .attr("x1", d => xScale(d))
            .attr("x2", d => xScale(d))
            .attr("y1", margin.bottom)
            .attr("y2", chartHeight)
            .style("stroke", "rgba(0,0,0,0.2)")
            .style("stroke-dasharray", "2,2");
      
        svg.append("g")
          .attr("transform", `translate(0,${chartHeight})`)
          .call(d3.axisBottom(xScale)
            .tickValues(d3.range(Math.floor(minYear / labelStep) * labelStep, maxYear, labelStep)) //---------------------------------------
            .tickFormat(d3.format(".0f"))
            .tickSizeOuter(0));
      

        for (let i = 0; i < data['machines'][i]['operations']; i++) {
            TimelineOne(data['machines'][i]['operations'], svg);
        }

        return svg.node();
}

function TimelineOne(data, svg) {
    
    // Create bars and labels
    const bars = svg.append("g")
      .selectAll("g")
      .data(data)
      .join("g");
  
    // Create bars
    bars.append("rect")
      .attr("x", d => xScale(d.birth))
      .attr("width", d => xScale(d.death) - xScale(d.birth))
      .attr("y", (d, i) => yScale(yPos[i]))
      .attr("height", barHeight)
      .attr("fill", "steelblue");
  
    // Create labels displaying only name
    bars.append("text")
      .text(d => d.name)
      .attr("x", d => xScale(d.birth) + 4)
      .attr("y", (d, i) => yScale(yPos[i]) + barHeight / 2)
      .attr("alignment-baseline", "central")
      .attr("font-size", 12)
      .attr("fill", "white")
      .attr("white-space", "nowrap")
      // .attr("overflow", "hidden")
      .attr("text-overflow", "ellipsis");
  
  
    // Return the SVG node as a value
    return svg.node();
}



function handleFileSelect(event) {
    const file = event.target.files[0];

    if (file) {
        const reader = new FileReader();
        reader.onload = function (e) {
            const fileContent = e.target.result;
            parsedData = parseFileFunc(fileContent);
            console.log(parsedData);

            // createGanttChart(parsedData);
            // Timeline(parsedData);

            const direction = "center";
            const timelineChart = Timeline(parsedData, direction);

            document.getElementById("gantt-chart").appendChild(timelineChart);

        };
        reader.readAsText(file);
    }
}