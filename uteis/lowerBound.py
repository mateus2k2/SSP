import loadData as ld
import os
from natsort import natsorted

folder_path = './input/MyInstancesDiferentToolSets'
files = os.listdir(folder_path)
files = natsorted(files)  # Natural sorting

fileWithPath = [f"{folder_path}/{file}" for file in files]

toolUnusedMap = ld.loadToolSet("./input/UnusedToolSetsClean.csv")
toolSetIndex = list(toolUnusedMap.keys())

print(f"duas maginas = {2*7} seis maginas = {6*7}")
for file in fileWithPath:
    data = ld.loadJobs(file)
    
    timeAcc = 0
    for item in data:
        timeAcc += item["Processing Time"]
    
    print(f"File: {file} = {timeAcc} minutes | {timeAcc/60} hours | {timeAcc/1440} days | dividido por 2 = {(timeAcc/1440)/2} | dividido por 6 = {(timeAcc/1440)/6}")
