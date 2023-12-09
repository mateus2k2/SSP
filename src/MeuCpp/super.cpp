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

    sort(originalJobs.begin(), originalJobs.end(), compareJobsSize);

    for(int i = 0; i < originalJobs.size(); i++){
        if (i == 249) cout << "i: " << i << endl;
        SuperToolSet SuperToolSetTmp;
        SuperJob SuperJobTmp;

        if(subSetsJobs[i] == false){
            
            SuperToolSetTmp.indexOriginalToolSet = originalJobs[i].indexToolSet;
            SuperToolSetTmp.originalToolSets.push_back(originalJobs[i].indexToolSet);

            SuperJobTmp.indexSuperToolSet = superToolSet.size();
            SuperJobTmp.originalJobs.push_back(i);
            SuperJobTmp.prioritySum = originalJobs[i].priority;
            SuperJobTmp.processingTimeSum = originalJobs[i].processingTime;

            for(int j = i; j < originalJobs.size(); j++){

                if ((isSubset(originalJobs[j].JobTools, originalJobs[i].JobTools)) && i != j && subSetsJobs[j] == false){
                // if ((isSubset(originalJobs[j].JobTools, originalJobs[i].JobTools) || originalJobs[j].indexToolSet == originalJobs[i].indexToolSet ) && i != j && subSetsJobs[j] == false){
                    SuperJobTmp.originalJobs.push_back(j);
                    SuperJobTmp.prioritySum += originalJobs[j].priority;
                    SuperJobTmp.processingTimeSum += originalJobs[j].processingTime;

                    if(find(SuperToolSetTmp.originalToolSets.begin(), SuperToolSetTmp.originalToolSets.end(), originalJobs[j].indexToolSet) == SuperToolSetTmp.originalToolSets.end() && subSetsToolSets[originalJobs[j].indexToolSet] == false){
                        SuperToolSetTmp.originalToolSets.push_back(originalJobs[j].indexToolSet);
                        subSetsToolSets[originalJobs[j].indexToolSet] = true;
                    }

                    subSetsJobs[j] = true;
                };
            }

            superJobs.push_back(SuperJobTmp);
            superToolSet.push_back(SuperToolSetTmp);


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