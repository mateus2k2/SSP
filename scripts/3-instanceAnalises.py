# analise de limite inferiro

import os
from natsort import natsorted
import uteis.loadData as ld

def lowerBound(folder_path):
    files = os.listdir(folder_path)
    files = natsorted(files)
    fileWithPath = [f"{folder_path}/{file}" for file in files]
    toolUnusedMap = ld.loadToolSet("./input/UnusedToolSetsClean.csv")

    print(f"duas maginas = {2*7} seis maginas = {6*7}")
    
    for i, file in enumerate(fileWithPath):
        data = ld.loadJobs(file)
        
        timeAcc = 0
        for item in data:
            timeAcc += item["Processing Time"]
        
        # print(f"File: {file} = {timeAcc} minutes | {timeAcc/60} hours | {timeAcc/1440} days | dividido por 2 = {(timeAcc/1440)/2} | dividido por 6 = {(timeAcc/1440)/6}")
        # print(f"File: {file} = {timeAcc/1440} days | dividido por 2 = {(timeAcc/1440)/2} | dividido por 6 = {(timeAcc/1440)/6}, avrg = {timeAcc/len(data)}, max = {max(data, key=lambda x: x['Processing Time'])['Processing Time']}")

        if i < 12:
            print(f"File: {file} = {timeAcc/1440:.2f} days | dividido por 2 = {(timeAcc/1440)/2:.2f} | avrg = {timeAcc/len(data):.2f} | max = {max(data, key=lambda x: x['Processing Time'])['Processing Time']:.2f}")
        else: 
            print(f"File: {file} = {timeAcc/1440:.2f} days | dividido por 6 = {(timeAcc/1440)/6:.2f} | avrg = {timeAcc/len(data):.2f} | max = {max(data, key=lambda x: x['Processing Time'])['Processing Time']:.2f}")

# ---------------------------------------------------------------------------------------------------
# MAIN
# ---------------------------------------------------------------------------------------------------

def main():
    folder_path = './input/MyInstancesSameToolSets'
    lowerBound(folder_path)

    print("\n\n\n")

    folder_path = './input/MyInstancesDiferentToolSets'
    lowerBound(folder_path)