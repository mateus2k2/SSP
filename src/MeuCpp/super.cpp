#include <vector>
#include <iostream>
#include <algorithm>

#include "headers/super.h"
#include "headers/GlobalVars.h"
#include <set>

using namespace std;

bool compareJobs(Job a, Job b) {
    return a.JobTools.size() > b.JobTools.size();
}

bool compareJobsPriority(int a, int b) {
    return originalJobs[a].priority > originalJobs[b].priority;
}

bool isSubset(const vector<int>& subset, const vector<int>& superset) {
    return includes(superset.begin(), superset.end(), subset.begin(), subset.end());
}

void makeSuper(){
    vector<bool> subSets(originalJobs.size(), false);

    sort(originalJobs.begin(), originalJobs.end(), compareJobs);

    for(int i = 0; i < originalJobs.size(); i++){
        SuperToolSet SuperToolSetTmp;
        SuperJob SuperJobTmp;

        if(subSets[i] == false){
            
            SuperToolSetTmp.indexOriginalToolSet = originalJobs[i].indexToolSet;
            SuperToolSetTmp.originalToolSets.push_back(originalJobs[i].indexToolSet);

            SuperJobTmp.indexSuperToolSet = superToolSet.size();
            SuperJobTmp.originalJobs.push_back(i);
            SuperJobTmp.prioritySum += originalJobs[i].priority;
            SuperJobTmp.processingTimeSum += originalJobs[i].processingTime;

            for(int j = i; j < originalJobs.size(); j++){

                if ((isSubset(originalJobs[j].JobTools, originalJobs[i].JobTools) || originalJobs[j].indexToolSet == originalJobs[i].indexToolSet ) && i != j){
                    SuperJobTmp.originalJobs.push_back(j);
                    SuperJobTmp.prioritySum += originalJobs[j].priority;
                    SuperJobTmp.processingTimeSum += originalJobs[j].processingTime;

                    if(find(SuperToolSetTmp.originalToolSets.begin(), SuperToolSetTmp.originalToolSets.end(), originalJobs[j].indexToolSet) != SuperToolSetTmp.originalToolSets.end()){
                        SuperToolSetTmp.originalToolSets.push_back(j);
                    }

                    subSets[j] = true;
                };
            }

            if(subSets[i] == false)

                superJobs.push_back(SuperJobTmp);
                superToolSet.push_back(SuperToolSetTmp);

                int indexSuperJob = superJobs.size();
                superJobs[indexSuperJob].originalJobs.sort(compareJobsPriority);
        }

    }

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