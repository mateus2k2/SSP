// /home/mateus/WSL/IC/SSP/input/ExemploArtigo/Jobs.csv;/home/mateus/WSL/IC/SSP/input/ExemploArtigo/ToolSets.csv
// 2;720;1440
// Machine: 0
// 1;1;0;180;1;1,2,3,4,5,12,13,14,
// 1;2;180;480;1;1,2,3,4,5,12,13,14,
// 2;1;480;900;1;4,12,13,14,15,16,17,18,
// 2;2;900;1450;1;4,12,13,14,15,16,17,18,
// 3;1;1450;1810;1;4,5,8,9,10,11,12,13,
// 3;2;1810;2290;1;4,5,8,9,10,11,12,13,
// Machine: 1
// 3;3;0;600;1;4,5,8,9,10,11,12,13,
// 4;1;600;840;1;5,12,13,14,15,16,17,18,
// 4;2;840;1380;1;5,12,13,14,15,16,17,18,
// 5;1;1440;1800;1;5,6,7,15,16,17,18,
// 6;1;1800;2400;1;5,6,15,16,17,18,19,
// END;11;17;5;1;233


function parseMachineSection(machineSection) {
    const lines = machineSection.trim().split('\n');
    const operations = lines.slice(1).map(line => line.split(';'));  // Changed to slice(1) to include the last line

    const operationsObj = operations.map(operation => (
        {
        'job': parseInt(operation[0]),
        'operation': parseInt(operation[1]),
        'start': parseInt(operation[2]),
        'end': parseInt(operation[3]),
        'priority': parseInt(operation[4]),
        'magazine': operation[5].slice(0, -1).split(',').map(num => parseInt(num))
        }
    ));

    return {
        operations: operationsObj,
    };
}


function parseFileFunc(fileContent) {
    let lines = fileContent.split('\n');
    let endInfo = lines.pop();  // Pop the last line (e.g. "END;11;17;5;1;233")

    const fileName = lines.shift();  // Remove the first line with file paths
    const header = lines.shift();  // Remove the second line (the header)

    // Parse the header
    const [planingHorizon, unsupervised, timescale] = header.split(';');
    const planejamentoObj = {
        'planingHorizon': parseInt(planingHorizon),
        'unsupervised': parseInt(unsupervised),
        'timescale': parseInt(timescale)
    };

    // Join the remaining lines to process machine sections
    const modifiedString = lines.join('\n');
    const machines = [];

    // Split by "Machine:" and process each machine's operations
    const machineSections = modifiedString.trim().split('Machine:');
    machineSections.slice(1).forEach(machineSection => {
        const { operations } = parseMachineSection(machineSection);
        machines.push(operations);
    });

    // Split the endInfo string by semicolon and parse the individual values
    const endInfoParts = endInfo.split(';');
    const endInfoObj = {
        'fineshedPriorityCount': parseInt(endInfoParts[1]),
        'switchs': parseInt(endInfoParts[2]),
        'switchsInstances': parseInt(endInfoParts[3]),
        'unfinesedPriorityCount': parseInt(endInfoParts[4]),
        'cost': parseInt(endInfoParts[5])
    };

    return {
        machines: machines,
        planejamentoObj: planejamentoObj,
        endInfo: endInfoObj
    };
}
