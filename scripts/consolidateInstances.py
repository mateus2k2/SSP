import os
from natsort import natsorted
import uteis.loadData as ld

def parseDataFile(fileName):
    metaData = {}

    with open(fileName, "r") as f:
        for line in f:
            line = line.strip()
            if line:
                key, value = line.split(" ")
                metaData[key] = value

    return metaData 

def createInstance(toolSets, jobs, metaData, title, output):
    normalizedUsedToolSets = {}
    normalizedUsedTools = {}
    boolLists = {}
    toolCount = 0

    for job in jobs:
        currentNormalizedToolSet = []
        
        for tool in toolSets[job["ToolSet"]]:
            if tool not in normalizedUsedTools:
                normalizedUsedTools[tool] = toolCount
                toolCount += 1
            currentNormalizedToolSet.append(normalizedUsedTools[tool])
        
        normalizedUsedToolSets[job["ToolSet"]] = sorted(currentNormalizedToolSet)

    for toolset in normalizedUsedToolSets:
        currantBoolList = [0] * toolCount
        for tool in normalizedUsedToolSets[toolset]:
            currantBoolList[tool] = 1
        boolLists[toolset] = currantBoolList

    outputFile = open(output, "w")

    parts = title.split('/')
    parameters = parts[-1].split(',')
    parameters = [param for param in parameters if param != 'v0']
    result = ','.join(parameters)

    outputFile.write(f"{result}\n")
    outputFile.write(f"{metaData['CAPACITY']}\n")
    outputFile.write(f"{metaData['MACHINES']}\n")
    outputFile.write(f"{metaData['DAYS']}\n")
    outputFile.write(f"{metaData['UNSUPERVISED_MINUTS']}\n\n")
    
    for job in jobs:
        outputFile.write(f"{job['Job']} {job['Operation']} {job['Priority']} {job['Processing Time']} ")
        outputFile.write(" ".join(str(x) for x in boolLists[job["ToolSet"]]))
        outputFile.write("\n")

def main(folder_name, toolSetFileName, output):
    files = os.listdir(folder_name)
    files = natsorted(files)
    filtered_files = [file for file in files if file.endswith(".csv")]
    files_with_path = [f"{folder_name}/{file}" for file in filtered_files]

    for i, file in enumerate(files_with_path):
        print(file)
        toolSets = ld.loadToolSet(toolSetFileName)
        jobs = ld.loadJobs(file)

        dataFile = file.replace(".csv", ".dat")
        metaData = parseDataFile(dataFile)

        createInstance(toolSets, jobs, metaData, file.replace(".csv", ""), output + str(i)+".txt")

if __name__ == "__main__":
    main("./input/MyInstancesSameToolSets", "./input/Processed/ToolSetInt.csv", "./input/Consolidated/SameToolSets/")
    main("./input/MyInstancesDiferentToolSets", "./input/Processed/ToolSetInt.csv", "./input/Consolidated/DiferentToolSets/")


# DAT FILE
# CAPACITY 80 
# MACHINES 2 
# DAYS 2 
# UNSUPERVISED_MINUTS 720