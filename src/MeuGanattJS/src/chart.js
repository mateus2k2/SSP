let parsedData = null;
const fileInput = document.getElementById('fileInput');
fileInput.addEventListener('change', handleFileSelect);

function Timeline(data) {
    const margin = ({ top: 10, right: 20, bottom: 50, left: 20 });

    const barHeight = 20;

    const labelStep = 720; //---------------------------------------

    const maxYear = data['planejamentoObj']["planingHorizon"] * data['planejamentoObj']['timescale'];
    const minYear = 0

    const width = maxYear + margin.left + margin.right;

    const yPos = Array.from({ length: 100 }, () => 50); //---------------------------------------

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


    for (let i = 0; i < data['machines'].length; i++) {
        // TimelineOne(data['machines'][i]['operations'], svg, yPos, barHeight);
        dataAtual = data['machines'][i]['operations'];

        const bars = svg.append("g")
            .selectAll("g")
            .data(dataAtual)
            .join("g");

        bars.append("rect")
            .attr("x", d => xScale(d["start"]))
            .attr("width", d => xScale(d["end"]) - xScale(d["start"]))
            .attr("y", (d, i) => yScale(yPos[i]))
            .attr("height", barHeight)
            .attr("fill", "steelblue");

        bars.append("text")
            .text(d => `${d["job"]} - ${d["operation"]}`)
            .attr("x", d => xScale(d["start"]) + 4)
            .attr("y", (d, i) => yScale(yPos[i]) + barHeight / 2)
            .attr("alignment-baseline", "central")
            .attr("font-size", 12)
            .attr("fill", "white")
            .attr("white-space", "nowrap")
            .attr("text-overflow", "ellipsis");
    }

    return svg.node();
}

function TimelineOne(data, svg, yPos, barHeight) {

    // Create bars and labels
    const bars = svg.append("g")
        .selectAll("g")
        .data(data)
        .join("g");

    // Create bars
    bars.append("rect")
        .attr("x", d => xScale(d["start"]))
        .attr("width", d => xScale(d["end"]) - xScale(d["start"]))
        .attr("y", (d, i) => yScale(yPos[i]))
        .attr("height", barHeight)
        .attr("fill", "steelblue");

    // Create labels displaying only name
    bars.append("text")
        .text(d => `${d["job"]} - ${d["operation"]}`)
        .attr("x", d => xScale(d["start"]) + 4)
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