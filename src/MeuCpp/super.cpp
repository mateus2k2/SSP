#include <vector>
#include <iostream>
#include <algorithm>

#include "headers/super.h"
#include "headers/GlobalVars.h"
#include <set>

using namespace std;

bool compareJobsSize(Job a, Job b) {
    return a.JobTools.size() > b.JobTools.size();
}

bool compareJobsPriority(int a, int b) {
    return originalJobs[a].priority > originalJobs[b].priority;
}

bool isSubset(const vector<int>& subset, const vector<int>& superset) {
    if (subset.size() == superset.size()) {
        return false;
    }

    return includes(superset.begin(), superset.end(), subset.begin(), subset.end());
}

map<int, bool> makeMapSubSetsToolSets(){
    map<int, bool> subSetsToolSets;
    for(int i = 0; i < originalToolSets.size(); i++){
        subSetsToolSets[originalToolSets[i].indexToolSet] = false;
    }
    return subSetsToolSets;
}

void makeSuper(){
    vector<bool> subSetsJobs(originalJobs.size(), false);
    map<int, bool> subSetsToolSets = makeMapSubSetsToolSets();
    
    vector<int> sameSuperToolSet(originalJobs.size(), -1);
    bool groopJobsWithSameToolSets = true;

    sort(originalJobs.begin(), originalJobs.end(), compareJobsSize);

    for(int i = 0; i < originalJobs.size(); i++){
        SuperToolSet SuperToolSetTmp;
        SuperJob SuperJobTmp;
        int superToolSetIndex = -1;

        if(subSetsJobs[i] == false){
            
            SuperToolSetTmp.indexOriginalToolSet = originalJobs[i].indexToolSet;
            SuperToolSetTmp.originalToolSets.push_back(originalJobs[i].indexToolSet);

            SuperJobTmp.originalJobs.push_back(i);
            if (sameSuperToolSet[i] == -1){
                superToolSetIndex = superToolSet.size();
                SuperJobTmp.indexSuperToolSet = superToolSetIndex;
            }
            else{
                superToolSetIndex = sameSuperToolSet[i];
                SuperJobTmp.indexSuperToolSet = superToolSetIndex;
            }
            SuperJobTmp.prioritySum = originalJobs[i].priority;
            SuperJobTmp.processingTimeSum = originalJobs[i].processingTime;

            for(int j = i; j < originalJobs.size(); j++){

                if ((isSubset(originalJobs[j].JobTools, originalJobs[i].JobTools) || (originalJobs[j].indexToolSet == originalJobs[i].indexToolSet && groopJobsWithSameToolSets)) && i != j && subSetsJobs[j] == false){
                    SuperJobTmp.originalJobs.push_back(j);
                    SuperJobTmp.prioritySum += originalJobs[j].priority;
                    SuperJobTmp.processingTimeSum += originalJobs[j].processingTime;

                    if(find(SuperToolSetTmp.originalToolSets.begin(), SuperToolSetTmp.originalToolSets.end(), originalJobs[j].indexToolSet) == SuperToolSetTmp.originalToolSets.end() && subSetsToolSets[originalJobs[j].indexToolSet] == false){
                        SuperToolSetTmp.originalToolSets.push_back(originalJobs[j].indexToolSet);
                        subSetsToolSets[originalJobs[j].indexToolSet] = true;
                    }

                    subSetsJobs[j] = true;
                };

                if ((originalJobs[j].indexToolSet == originalJobs[i].indexToolSet) && i != j && subSetsJobs[j] == false){
                    sameSuperToolSet[j] = superToolSetIndex;
                };

            }

            superJobs.push_back(SuperJobTmp);
            if (sameSuperToolSet[i] == -1) superToolSet.push_back(SuperToolSetTmp);

            int indexSuperJob = superJobs.size();
            superJobs[indexSuperJob-1].originalJobs.sort(compareJobsPriority);


        }

    }

    //ATUALIZAR NUMBER OF TOOLS
    set <int> allTools;
    for(int i = 0; i < superToolSet.size(); i++){
        for(int j = 0; j < originalToolSets[superToolSet[i].indexOriginalToolSet].tools.size(); j++){
            allTools.insert(originalToolSets[superToolSet[i].indexOriginalToolSet].tools[j]);
        }
    }
    numberTools = allTools.size();
    
    //ATUALIZAR NUMBER OF JOBS
    numberJobs = superJobs.size();

    // cout << "SUBSETSJOBS: " << subSetsJobs.size() << endl;
    // int countTrueJobs = 0;
    // for (auto it = subSetsJobs.begin(); it != subSetsJobs.end(); ++it){
    //     cout << *it << " ";
    //     if ((*it) == true) countTrueJobs++;
    // }
    // cout << endl;
    // cout << "NUMERO DE VERDADEIROS: " << countTrueJobs << endl << endl;

    // cout << "SUBSETSTOOLSETS: " << subSetsToolSets.size() << endl;
    // int countTrueToolSets = 0;
    // for (auto it = subSetsToolSets.begin(); it != subSetsToolSets.end(); ++it){
    //     cout << it->second << " ";
    //     if (it->second == true) countTrueToolSets++;
    // }
    // cout << endl;
    // cout << "NUMERO DE VERDADEIROS: " << countTrueToolSets << endl << endl;

}

void makePriorityIndex(){
    for(int i = 0; i < superJobs.size(); i++){
        for (auto it = superJobs[i].originalJobs.begin(); it != superJobs[i].originalJobs.end(); ++it){
            if(originalJobs[*it].priority == 1){
                int originalIndex = distance(superJobs[i].originalJobs.begin(), it);
                pair<int,int> tmp(i,originalIndex);
                priorityIndex.insert(tmp);
            }

        }
        
    }

}