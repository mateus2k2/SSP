import reportParser as rp
import loadData as ld
import os
from natsort import natsorted

# folder_path = './output/MyInstancesSameToolSets'
folder_path = './output/MyInstancesDiferentToolSets'
files = os.listdir(folder_path)
files = natsorted(files)  # Natural sorting

fileWithPath = [f"{folder_path}/{file}" for file in files]

for file in fileWithPath:
    planejamento, machines, endInfo = rp.parseReport(file)
    # print(f'{file} = {endInfo}')
    print(f'{((endInfo["timeSpent"]) / 60000):.2f}')