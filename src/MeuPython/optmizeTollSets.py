import random
import pandas as pd
import json
import numpy as np

def stringToInt():
    df = pd.read_csv('/home/mateus/WSL/IC/data/Data Tool Set Compositions.csv', sep=';', header=None)

    df = df.fillna('NaNPlaceholder')
    df = df.drop(0, axis=1)
    df.index = range(1, len(df) + 1)
    
    uniqueStrings = df.stack().unique()
    stringToIntMapping = {string: i+1 for i, string in enumerate(uniqueStrings)}
    stringToIntMapping['NaNPlaceholder'] = ''

    df = df.map(lambda x: stringToIntMapping[x])

    df.to_csv('/home/mateus/WSL/IC/data/Data Tool Set Compositions Int.csv', index=True, header=False, sep=';')
    
    with open('data.json', 'w') as fp:
        json.dump(stringToIntMapping, fp)
        
def pruneNotNeaded():
    JobsAll1 = pd.read_csv("/home/mateus/WSL/IC/data/Data Jobs 250.csv", delimiter=';', dtype=int)
    JobsAll2 = pd.read_csv("/home/mateus/WSL/IC/data/Data Jobs 750.csv", delimiter=';', dtype=int)
    JobsAll3 = pd.read_csv("/home/mateus/WSL/IC/data/Data Jobs 1000.csv", delimiter=';', dtype=int)

    toolSetNeaded1 = []
    toolSetNeaded2 = []
    toolSetNeaded3 = []

    for i in range(len(JobsAll1)):
        toolSetNeaded1.append(JobsAll1['ToolSet'][i])
    for i in range(len(JobsAll2)):
        toolSetNeaded2.append(JobsAll2['ToolSet'][i])
    for i in range(len(JobsAll3)):
        toolSetNeaded3.append(JobsAll3['ToolSet'][i])
    
    toolSetNeaded = set(toolSetNeaded1 + toolSetNeaded2 + toolSetNeaded3)

    # print(len(toolSetNeaded))

    df = pd.read_csv('/home/mateus/WSL/IC/data/Data Tool Set Compositions Int.csv', sep=';', header=None)
    df_filtered = df[df[0].isin(toolSetNeaded)]
    df_filtered = df_filtered.map(lambda x: '' if pd.isna(x) else int(x))
    df_filtered.to_csv('/home/mateus/WSL/IC/data/Data Tool Set Compositions Filter.csv', index=False, sep=';', header=False)

def countMaiores80():
    ToolSetAll = pd.read_csv("/home/mateus/WSL/IC/data/Data Tool Set Compositions.csv", delimiter=';',header=None, dtype=str)

    toolSets = ToolSetAll.values.tolist()
    toolSets = [[int(x) for x in row if x == x] for row in toolSets]

    maiores = []
    
    # Depois de limpar os toolSets nescessarios apenas para os Jobs de cada instancia 
    print("ToolsSets Maiores que 80")
    for toolset in toolSets:
        if len(toolset) - 1 > 80:
            print(f'Indice do ToolSet: {toolset[0]}, size {len(toolset)-1}')
            maiores.append(toolset[0])
    
    JobsAll1 = pd.read_csv("/home/mateus/WSL/IC/data/Data Jobs 250.csv", delimiter=';', dtype=int)
    JobsAll2 = pd.read_csv("/home/mateus/WSL/IC/data/Data Jobs 750.csv", delimiter=';', dtype=int)
    JobsAll3 = pd.read_csv("/home/mateus/WSL/IC/data/Data Jobs 1000.csv", delimiter=';', dtype=int)

    toolSetNeaded1 = ([], [], [], [])
    toolSetNeaded2 = ([], [], [], [])
    toolSetNeaded3 = ([], [], [], [])

    for i in range(len(JobsAll1)):
        toolSetNeaded1[0].append(JobsAll1['Operation'][i])
        toolSetNeaded1[1].append(JobsAll1['Job'][i])
        toolSetNeaded1[2].append(JobsAll1['ToolSet'][i])
        toolSetNeaded1[3].append(JobsAll1['Processing Time'][i])
    for i in range(len(JobsAll2)):
        toolSetNeaded2[0].append(JobsAll2['Operation'][i])
        toolSetNeaded2[1].append(JobsAll2['Job'][i])
        toolSetNeaded2[2].append(JobsAll2['ToolSet'][i])
        toolSetNeaded2[3].append(JobsAll2['Processing Time'][i])
    for i in range(len(JobsAll3)):
        toolSetNeaded3[0].append(JobsAll3['Operation'][i])
        toolSetNeaded3[1].append(JobsAll3['Job'][i])
        toolSetNeaded3[2].append(JobsAll3['ToolSet'][i])
        toolSetNeaded3[3].append(JobsAll3['Processing Time'][i])
       
    print("\nFile 250\n")
    for i, item in enumerate(toolSetNeaded1[2]):
        if item in maiores:
            print(f'Operation {toolSetNeaded1[0][i]} Job {toolSetNeaded1[1][i]} ToolSet {toolSetNeaded1[2][i]} Processing Time {toolSetNeaded1[3][i]} ')
    
    print("\nFile 750\n")
    for i, item in enumerate(toolSetNeaded2[2]):
        if item in maiores:
            print(f'Operation {toolSetNeaded2[0][i]} Job {toolSetNeaded2[1][i]} ToolSet {toolSetNeaded2[2][i]} Processing Time {toolSetNeaded2[3][i]} ')
            
    print("\nFile 1000\n")
    for i, item in enumerate(toolSetNeaded3[2]):
        if item in maiores:
            print(f'Operation {toolSetNeaded3[0][i]} Job {toolSetNeaded3[1][i]} ToolSet {toolSetNeaded3[2][i]} Processing Time {toolSetNeaded3[3][i]} ')

def subSets():
    ToolSetAll = pd.read_csv("/home/mateus/WSL/IC/data/Data Tool Set Compositions.csv", delimiter=';', header=None, dtype=str)

    toolSets = ToolSetAll.values.tolist()
    toolSets = [[int(x) for x in row if x == x] for row in toolSets]

    index = [item.pop(0) for item in toolSets]
    sets = [set(x) for x in toolSets]

    subsets_count = {} 
    
    def jaccard_similarity(set1, set2):
        intersection = len(set1.intersection(set2))
        union = len(set1.union(set2))
        return intersection / union

    for i, item1 in enumerate(sets):
        print("\n------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n")
        print(f'{i}/{len(sets)}: Len: {len(item1)} index: {index[i]} \n')
        for j, item2 in enumerate(sets):
            if i != j:
                if item2.issubset(item1):
                    print(f'*** SUBSET = {index[j]} | {index[i]} ***')
                    print(f'Jacard Similarity {index[i]} - {index[j]}: {jaccard_similarity(item1, item2)}')
                    if index[i] not in subsets_count:
                        subsets_count[index[i]] = 1
                    else:
                        subsets_count[index[i]] += 1
                    print()
                else:
                    print(f'NOT SUBSET = {index[j]} | {index[i]}')
                    print(f'Jacard Similarity {index[j]} - {index[i]}: {jaccard_similarity(item2, item1)}')
                    print()

    print("Subset counts:")
    for subset, count in subsets_count.items():
        print(f"{subset}: {count} times")
        
                
                
# stringToInt()
# pruneNotNeaded()
# countMaiores80()
subSets()