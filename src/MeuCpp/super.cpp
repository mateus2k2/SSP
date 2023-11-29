#include <vector>
#include <iostream>

#include "headers/super.h"
#include "headers/GlobalVars.h"

using namespace std;

// def subSets():
//     ToolSetAll = pd.read_csv("/home/mateus/WSL/IC/data/ToolSetAll.csv", delimiter=';', header=None, dtype=str)

//     toolSets = ToolSetAll.values.tolist()
//     toolSets = [[int(x) for x in row if x == x] for row in toolSets]

//     toolSets.sort(key=len, reverse=True)
//     index = [item.pop(0) for item in toolSets]
//     sets = [set(x) for x in toolSets]
    
//     subsets_count = {}
    
//     for i in range(0, len(sets)):
//         countAtual = 0
//         print(f'QUEM É SUBSET DO {index[i]} Tamanho: {len(sets[i])}\n')
//         for j in range(i, len(sets)):
//             if (sets[j].issubset(sets[i])) and (i != j ) and (sets[i] != sets[j]):
//                 countAtual += 1
//                 print(f'{index[j]} é subset de {index[i]}')
//         subsets_count[index[i]] = countAtual
//         print(f'\n----------------------------------------------------------------------------\n')


// Verificar os subconjuntos de cada ToolSet
// Pegar os jobs que usam esses toolsSets que sao sub conjuntos
// Agora esses jobs vao usar esse toolSet maior  
// Fazer um unico job (Somar o tempo de processamento)

// Como representar esses jobs
// Como mapear de volta para os jobs originais
// Como mapear de volta para os toolSets originais


void loadDataTypes(){
    vector<Job> jobsType;
    vector<ToolSet> ToolSetsType;

    for (int i = 0; i < numberJobs; i++){
        Job jobTmp;

        jobTmp.indexJob = job[i];
        jobTmp.indexOperation = operation[i];
        jobTmp.indexToolSet = JobToolsIndex[i];
        jobTmp.processingTime = processingTime[i];
        jobTmp.priority = priority[i];
        jobTmp.indexMachine = -1;

        jobTmp.JobTools = JobTools[i];

        jobTmp.superJob = false;
        jobTmp.originalJobs = {};

        jobsType.push_back(jobTmp);
    }

    for (int i = 0; i < JobToolsIndex.size(); i++){
        ToolSet toolSetTmp;

        toolSetTmp.indexToolSet = JobToolsIndex[i];
        toolSetTmp.tools = mapToolSets[JobToolsIndex[i]];

        toolSetTmp.superToolSet = false;
        toolSetTmp.originalToolSets = {};

        ToolSetsType.push_back(toolSetTmp);
    }


    // Print the result

    for (int i = 0; i < jobsType.size(); i++){
        cout << "Job: " << jobsType[i].indexJob << "\nOperation: " << jobsType[i].indexOperation << "\nToolSet: " << jobsType[i].indexToolSet << "\nProcessingTime: " << jobsType[i].processingTime << "\nPriority: " << jobsType[i].priority << "\nMachine: " << jobsType[i].indexMachine << "\nJobTools: " <<  endl;
        
        for(int j = 0; j < jobsType[i].JobTools.size(); j++){
            cout << jobsType[i].JobTools[j] << " ";
        }

        cout << "\n\n";
    }

    for(int i = 0; i < ToolSetsType.size(); i++){
        cout << "ToolSet: " << ToolSetsType[i].indexToolSet << "\nTools: ";

        for(int j = 0; j < ToolSetsType[i].tools.size(); j++){
            cout << ToolSetsType[i].tools[j] << " ";
        }
        
        cout << "\n\n";
    }

    // ------------------------------------------------------------------------------------------------------------------------------------------------------

}