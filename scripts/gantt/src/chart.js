function Timeline(data) {
    const margin = ({ top: 50, right: 20, bottom: 50, left: 80 });


    const maxYear = data['planejamentoObj']["planingHorizon"] * data['planejamentoObj']['timescale'];
    const minYear = 0;

    const yPosMax = data['machines'].length * 2;
    const yPosMin = -1;

    const barHeight = 80;
    const chartHeight = (yPosMax - yPosMin) * barHeight * 2;

    // const width = 1100;
    const width = 1500; // Increase width for more horizontal space
    const height = chartHeight + margin.top + margin.bottom;
    // const height = chartHeight + margin.top + margin.bottom + 100; // Increase height to accommodate more vertical space


    const xScale = d3.scaleLinear().domain([minYear, maxYear]).range([margin.left, width - margin.right]);
    const yScale = d3.scalePoint()
        .domain(d3.range(yPosMin + 1, data['machines'].length))
        .range([height - margin.bottom, margin.top])
        .padding(1.5);

        
    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


    const realThing = d3.select(document.createElementNS("http://www.w3.org/2000/svg", "svg"))
        .attr("width", width)
        .attr("height", height);

    const clip = realThing.append("defs").append("clipPath")
        .attr("id", "clip")
        .append("rect")
        .attr("x", margin.left)
        .attr("y", margin.top)
        .attr("width", width - margin.left - margin.right)
        .attr("height", chartHeight);

    const svg = realThing.append("g")
        .attr("clip-path", "url(#clip)")

    
    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // let yPos = data['machines'].length-1;
    for (let i = 0; i < data['machines'].length; i++) {
        dataAtual = data['machines'][i];

        const yPos = i;

        //Group for the bars and text
        const bars = svg.append("g")
        .selectAll("g")
        .data(dataAtual)
        .join("g")
        .on("click", handleClick);
    
    // Add the bars to the chart
    bars.append("rect")
        .attr("x", d => xScale(d["start"]))
        .attr("width", d => {
            const width = xScale(d["end"]) - xScale(d["start"]);
            return width > 0 ? width : 0;  // Ensure non-negative width
        })
        .attr("y", (d, i) => yScale(yPos))
        .attr("height", barHeight)
        .attr("fill", "steelblue")
        .attr("class", "chart-block")
        .attr("stroke", "black")
        .attr("stroke-width", 1)
        .on("click", handleClick);
    
    // Add the foreignObject with the div inside
    const groupDivs1 = bars.append("foreignObject")
        .attr("x", d => xScale(d["start"]))
        .attr("y", (d, i) => yScale(yPos))
        .attr("width", d => {
            const width = xScale(d["end"]) - xScale(d["start"]);
            return width > 0 ? width : 0;  // Ensure non-negative width
        })
        .attr("height", barHeight)
        .append("xhtml:div")
        .style("width", d => {
            const width = xScale(d["end"]) - xScale(d["start"]);
            return width > 0 ? width + "px" : "0px";  // Ensure non-negative width
        })
        .style("height", barHeight + "px")
        .style("text-anchor", "middle")
        .style("alignment-baseline", "middle")
        .style("white-space", "nowrap")
        .style("font-weight", "bold")
        .attr("class", "chart-block-div")
        .on("click", handleClick);
    
    // Add the paragraph text inside the div
    groupDivs1.append("p")
        .text(d => `(${d["job"]}, ${d["operation"]})`)
        .on("click", handleClick);
    }



    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


    //Calculation the setps based on the timescale and unsupervised
    const timescale = data.planejamentoObj.timescale;
    const unsupervised = data.planejamentoObj.unsupervised;
    const planningHorizon = data.planejamentoObj.planingHorizon;
    let accumulatedValue = 0;

    const increments = [timescale - unsupervised, unsupervised];
    const steplist = Array.from({ length: planningHorizon * 2 + 1 }, (_, i) => {
        if (i > 0) accumulatedValue += increments[i % 2];
        return accumulatedValue;
    });

    // Add the lines to the chart
    const linesLayer = svg.append("g").attr("class", "lines-layer");
    linesLayer.append("g")
        .selectAll("line")
        .data(steplist)
        .join("line")
        .attr("x1", d => xScale(d))
        .attr("x2", d => xScale(d))
        .attr("y1", margin.bottom)
        .attr("y2", chartHeight)
        .style("stroke", "rgba(0,0,0,0.2)")
        .style("stroke-dasharray", "2,2");

    // Add the x-axis to the chart with the ticks
    svg.append("g")
        .attr("class", "axis axis--x")
        .style("font-size", "15px")
        .attr("transform", `translate(0,${chartHeight})`)
        .call(d3.axisBottom(xScale)
            .tickValues(steplist)
            .tickFormat(d3.format(".0f"))
            .tickSizeOuter(0));

    // Add the unsupervised areas to the chart
    for (let i = 1; i < steplist.length; i += 2) {
        svg.append("rect")
            .data([{ start: steplist[i], end: steplist[i + 1] }])
            .attr("x", xScale(steplist[i]))
            .attr("y", 0)
            .attr("width", xScale(steplist[i + 1]) - xScale(steplist[i]))
            .attr("height", chartHeight)
            .attr("fill", "lightgray")
            .attr("opacity", 0.5)
            .attr("class", "usupervised-area")
            .style("pointer-events", "none")
    }


    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


    const machineLabels = Array.from({ length: data['machines'].length }, (_, i) => {
        return "Maquina " + (i + 1);
    });

    realThing.append("g")
        .attr("class", "axis axis--y")
        .style("font-size", "15px")
        .attr("transform", `translate(${margin.left},0)`)
        .call(d3.axisLeft(yScale).tickSizeOuter(0).tickFormat((d, i) => machineLabels[i]));

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


    // Zoom difinitions
    const zoom = d3.zoom()
        .scaleExtent([-32, 320])
        .on("zoom", zoomed);


    function zoomed(event) {
        const new_xScale = event.transform.rescaleX(xScale);

        // Update the lines
        linesLayer.select("g").selectAll("line")
            .attr("x1", d => new_xScale(d))
            .attr("x2", d => new_xScale(d));

        // Update the recs 
        svg.selectAll("rect.chart-block")
            .attr("x", d => new_xScale(d["start"]))
            .attr("width", d => {
                const width = new_xScale(d["end"]) - new_xScale(d["start"]);
                return width > 0 ? width : 0;  // Ensure non-negative width
            });

        // Update the from the unsipervised areas
        svg.selectAll("rect.usupervised-area")
            .attr("x", d => new_xScale(d["start"]))
            .attr("width", d => new_xScale(d["end"]) - new_xScale(d["start"]));

        // Update the text (the divs)
        svg.selectAll("foreignObject")
            .attr("x", d => new_xScale(d["start"]))
            .attr("width", d => {
                const width = new_xScale(d["end"]) - new_xScale(d["start"]);
                return width > 0 ? width : 0;  // Ensure non-negative width
            })
            .select("div.chart-block-div")
            .style("width", d => {
                const width = new_xScale(d["end"]) - new_xScale(d["start"]);
                return width > 0 ? width + "px" : "0px";  // Ensure non-negative width
            });

        realThing.select(".axis--x").call(d3.axisBottom(new_xScale).tickValues(steplist).tickFormat(d3.format(".0f")).tickSizeOuter(0));
    }


    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    realThing.call(zoom);

    // ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    return realThing.node();

}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


// Click handler
function handleClick(event, d) {

    let containsObject = clicked.some(item => JSON.stringify(item) === JSON.stringify(d));
    if (containsObject) {
        return;
    }

    var newInfo = document.createElement('pre');
    newInfo.id = 'json-renderer';

    newInfo.textContent = JSON.stringify(d, undefined, 2);
    $(newInfo).jsonViewer(d, { collapsed: true });
    document.getElementById("info-cards").appendChild(newInfo);

    clicked.push(d);

}



// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



// File handler
function handleFileSelect(event) {
    const file = event.target.files[0];

    if (file) {
        const reader = new FileReader();
        reader.onload = function (e) {
            const fileContent = e.target.result;
            parsedData = parseFileFunc(fileContent);
            console.log(parsedData);

            const direction = "center";
            const timelineChart = Timeline(parsedData, direction);

            document.getElementById("gantt-chart").appendChild(timelineChart);

        };
        reader.readAsText(file);
    }
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



let clicked = [];
const fileInput = document.getElementById('fileInput');
fileInput.addEventListener('change', handleFileSelect);
