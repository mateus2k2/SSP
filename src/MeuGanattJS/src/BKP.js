let parsedData = null;
const fileInput = document.getElementById('fileInput');
fileInput.addEventListener('change', handleFileSelect);










function Timeline(data) {
    const margin = ({ top: 10, right: 20, bottom: 50, left: 20 });

    const barHeight = 20;

    const maxYear = data['planejamentoObj']["planingHorizon"] * data['planejamentoObj']['timescale'];
    const minYear = 0;

    const width = 1500;

    const yPosMax = data['machines'].length * 2;
    const yPosMin = -1;
    const chartHeight = (yPosMax - yPosMin) * barHeight * 2;
    const height = chartHeight + margin.top + margin.bottom;

    const xScale = d3.scaleLinear().domain([minYear, maxYear]).range([margin.left, width - margin.right]);
    const yScale = d3.scalePoint().domain(d3.range(yPosMin, yPosMax + 1)).range([height - margin.bottom, margin.top]).padding(1.5);

    const svg = d3.select(document.createElementNS("http://www.w3.org/2000/svg", "svg"))
        .attr("width", width)
        .attr("height", height);

    const timescale = data.planejamentoObj.timescale;
    const unsupervised = data.planejamentoObj.unsupervised;
    const planningHorizon = data.planejamentoObj.planingHorizon;
    let accumulatedValue = 0;

    const increments = [timescale - unsupervised, unsupervised];
    const steplist = Array.from({ length: planningHorizon * 2 + 1 }, (_, i) => {
        if (i > 0) accumulatedValue += increments[i % 2];
        return accumulatedValue;
    });

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

    svg.append("g")
        .attr("class", "axis axis--x")
        .attr("transform", `translate(0,${chartHeight})`)
        .call(d3.axisBottom(xScale)
            .tickValues(steplist)
            .tickFormat(d3.format(".0f"))
            .tickSizeOuter(0));

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
    }


    for (let i = 0; i < data['machines'].length; i++) {
        dataAtual = data['machines'][i]['operations'];

        const yPos = i;

        const bars = svg.append("g")
            .selectAll("g")
            .data(dataAtual)
            .join("g")
            .on("click", handleClick);


        bars.append("rect")
            .attr("x", d => xScale(d["start"]))
            .attr("width", d => xScale(d["end"]) - xScale(d["start"]))
            .attr("y", (d, i) => yScale(yPos))
            .attr("height", barHeight)
            .attr("fill", "steelblue")
            .attr("class", "chart-block")
            .attr("stroke", "black")
            .attr("stroke-width", 1)
            .on("click", handleClick);

        bars.append("text")
            .text(d => `${d["job"]} - ${d["operation"]}`)
            .attr("x", d => xScale((d["start"] + d["end"]) / 2))
            .attr("y", (d, i) => yScale(yPos) + barHeight / 2)
            .attr("text-anchor", "middle") // Center the text horizontally
            .attr("dominant-baseline", "middle") // Center the text vertically
            .attr("font-size", 12)
            .attr("fill", "white")
            .attr("white-space", "nowrap")
            .attr("text-overflow", "ellipsis")
            .attr("class", "chart-block-text")
            .on("click", handleClick);

    }





    const zoom = d3.zoom()
        .scaleExtent([-32, 320])
        .on("zoom", zoomed);

    svg.call(zoom);

    function zoomed(event) {
        const new_xScale = event.transform.rescaleX(xScale);

        linesLayer.select("g").selectAll("line")
            .attr("x1", d => new_xScale(d))
            .attr("x2", d => new_xScale(d));

        svg.selectAll("rect.chart-block")
            .attr("x", d => new_xScale(d["start"]))
            .attr("width", d => new_xScale(d["end"]) - new_xScale(d["start"]));

        svg.selectAll("rect.usupervised-area")
            .attr("x", d => new_xScale(d["start"]))
            .attr("width", d => new_xScale(d["end"]) - new_xScale(d["start"]));

        svg.selectAll("text")
            .attr("x", function (d) {
                return new_xScale((d["start"] + d["end"]) / 2);
            });

        svg.select(".axis--x").call(d3.axisBottom(new_xScale).tickValues(steplist).tickFormat(d3.format(".0f")).tickSizeOuter(0));


    }

    return svg.node();

}

let clicked = [];

function handleClick(event, d) {
    console.log("Bar Clicked:", d);

    let containsObject = clicked.some(item => JSON.stringify(item) === JSON.stringify(d));
    if (containsObject) {
        console.log("Already clicked");
        return;
    }

    let newInfo = document.getElementById('json-renderer').cloneNode();
    newInfo.textContent = JSON.stringify(d, undefined, 2);
    $(newInfo).jsonViewer(d, { collapsed: true });
    document.getElementById("info-cards").appendChild(newInfo);

    clicked.push(d);

}










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