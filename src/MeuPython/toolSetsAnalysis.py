import pandas as pd
import matplotlib.pyplot as plt


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
    ToolSetAll = pd.read_csv("/home/mateus/WSL/IC/data/1000ToolSet.csv", delimiter=';', header=None, dtype=str)

    toolSets = ToolSetAll.values.tolist()
    toolSets = [[int(x) for x in row if x == x] for row in toolSets]

    toolSets.sort(key=len, reverse=True)
    index = [item.pop(0) for item in toolSets]
    sets = [set(x) for x in toolSets]
    
    subsets_count = {}
    
    for i in range(0, len(sets)):
        countAtual = 0
        print(f'QUEM É SUBSET DO {index[i]} Tamanho: {len(sets[i])}\n')
        for j in range(i, len(sets)):
            if (sets[j].issubset(sets[i])) and (i != j ) and (sets[i] != sets[j]):
                countAtual += 1
                print(f'{index[j]} é subset do {index[i]}')
        subsets_count[index[i]] = countAtual
        print(f'\n----------------------------------------------------------------------------\n')
    
    # index.sort()
    # listaBoa = [subsets_count[x] for x in index]
    
    # plt.figure(figsize=(10, 5))
    # plt.subplot(2, 1, 1)
    # plt.plot(index, listaBoa, marker='o')
    # plt.title('Number of Subsets for Each ToolSet')
    # plt.xlabel('ToolSet Index (Sequence Number)')
    # plt.ylabel('Number of Subsets')
    
    # plt.tight_layout()
    # plt.show()

subSets()