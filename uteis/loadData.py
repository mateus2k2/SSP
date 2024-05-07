import pandas as pd

def loadToolSet(fileName):
    toolSetsFilePath = f'/home/mateus/WSL/IC/SSP/input/{fileName}'
    toolSetsDF = pd.read_csv(toolSetsFilePath, header=None, delimiter=';')
    index = toolSetsDF.iloc[:, 0]
    toolSetsDF = toolSetsDF.drop(toolSetsDF.columns[0], axis=1)
    toolSetsDF = toolSetsDF.fillna('NaNPlaceholder')
    toolSetsDict = toolSetsDF.to_dict(orient='records')
    toolSetList = []
    for i in range(0, len(toolSetsDict)):
        toolSetList.append([])
        for coisa in toolSetsDict[i].values():
            if coisa != 'NaNPlaceholder':
                toolSetList[i].append(int(coisa))
    
    toolSetMap = {indexAtual : toolSetAtual for indexAtual, toolSetAtual in zip(index, toolSetList)}
    return toolSetMap

def loadJobs(fileName):
    jobsFilePath = f'/home/mateus/WSL/IC/SSP/input/{fileName}'
    jobsDF = pd.read_csv(jobsFilePath, delimiter=';')
    jobsDict = jobsDF.to_dict(orient='records')
    return jobsDict