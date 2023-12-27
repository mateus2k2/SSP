
function parseMachineSection(machineSection) {
    const lines = machineSection.trim().split('\n');
    const machineInfo = lines[0].split('=')[1].trim();
    const operations = lines.slice(1, -1).map(line => line.split(';'));
    const endInfo = lines[lines.length - 1].split(';');

    const operationsObj = operations.map(operation => ({
        'job': parseInt(operation[0]),
        'operation': parseInt(operation[1]),
        'start': parseInt(operation[2]),
        'end': parseInt(operation[3]),
        'priority': parseInt(operation[4]),
        'maganize': operation[5].slice(0, -1).split(',').map(num => parseInt(num))
    }));

    const machineInfoObj = machineInfo.slice(0, -1).split(";").map(pair => {
        const pairList = pair.split(',')
        pairList[0] = pairList[0].slice(1)
        pairList[1] = pairList[1].slice(0, -1)

        pairList[0] = parseInt(pairList[0])
        pairList[1] = parseInt(pairList[1])

        return {
            'job': pairList[0],
            'operation': pairList[1]
        };
    });

    const endInfoObj = {
        'fineshedPriorityCount': parseInt(endInfo[1]),
        'switchs': parseInt(endInfo[2]),
        'switchsInstances': parseInt(endInfo[3]),
        'unfinesedPriorityCount': parseInt(endInfo[4]),
        'cost': parseInt(endInfo[5])
    };

    return {
        machineInfo: machineInfoObj,
        operations: operationsObj,
        endInfo: endInfoObj
    };
}

function parseFileFunc(fileContent) {
    const lines = fileContent.split('\n');
    const header = lines.shift();
    const [planingHorizon, unsupervised, timescale] = header.split(';');
    const planejamentoObj = {
        'planingHorizon': parseInt(planingHorizon),
        'unsupervised': parseInt(unsupervised),
        'timescale': parseInt(timescale)
    };

    const modifiedString = lines.join('\n');

    const machines = [];
    const machineSections = modifiedString.trim().split('Machine:');
    machineSections.slice(1).forEach(machineSection => {
        const { machineInfo, operations, endInfo } = parseMachineSection(machineSection);
        machines.push({
            'machine_info': machineInfo,
            'operations': operations,
            'end_info': endInfo
        });
    });

    return {
        machines: machines,
        planejamentoObj: planejamentoObj
    };
}