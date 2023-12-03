#include <vector>
#include <iostream>
#include <algorithm>

#include "headers/super.h"
#include "headers/GlobalVars.h"
#include <set>

using namespace std;

void loadDataTypes(){

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

    for (auto it = mapToolSets.begin(); it != mapToolSets.end(); ++it) {
        ToolSet toolSetTmp;

        toolSetTmp.indexToolSet = it->first;
        toolSetTmp.tools = it->second;

        toolSetTmp.superToolSet = false;
        toolSetTmp.originalToolSets = {};

        ToolSetsType.push_back(toolSetTmp);
    }

    // ------------------------------------------------------------------------------------------------------------------------------------------------------

}
bool compareFunction(ToolSet a, ToolSet b) {
    return a.tools.size() > b.tools.size();
}

bool isSubset(const std::vector<int>& subset, const std::vector<int>& superset) {
    return std::includes(superset.begin(), superset.end(), subset.begin(), subset.end());
}

void removeSubSets(){
    
    // ------------------------------------------------------------------------------------------------------------------------------------------------------

    sort(ToolSetsType.begin(), ToolSetsType.end(), compareFunction);

    int countToolSetsTypeDeletados = 0;
    int countjobsTypeDeletados = 0;

    for(int i = 0; i < ToolSetsType.size(); i++){
        vector<int> indexTmp;

        for(int j = 0; j < ToolSetsType.size(); j++){
            if (isSubset(ToolSetsType[j].tools, ToolSetsType[i].tools) && i != j /* && ToolSetsType[j].indexToolSet != ToolSetsType[i].indexToolSet */){
                ToolSetsType[i].superToolSet = true;
                ToolSetsType[i].originalToolSets.push_back(countToolSetsTypeDeletados++);
                ToolSetsTypeDeletados.push_back(ToolSetsType[j]);

                indexTmp.push_back(ToolSetsType[j].indexToolSet);

                ToolSetsType.erase(ToolSetsType.begin() + j);

            };
        }
        if (ToolSetsType[i].superToolSet){
            indexTmp.push_back(ToolSetsType[i].indexToolSet);

            Job superJob;

            superJob.indexJob = numberJobs + countjobsTypeDeletados + 1;
            superJob.indexOperation = 0;
            superJob.indexToolSet = ToolSetsType[i].indexToolSet;
            superJob.processingTime = 0;
            superJob.priority = -1;
            superJob.indexMachine = -1;

            superJob.JobTools = ToolSetsType[i].tools;

            superJob.superJob = true;
            superJob.originalJobs = {};

            for(int j = 0; j < jobsType.size(); j++){
                if (find(indexTmp.begin(), indexTmp.end(), jobsType[j].indexToolSet) != indexTmp.end()){
                    superJob.originalJobs.push_back(countjobsTypeDeletados++);

                    superJob.processingTime += jobsType[j].processingTime;

                    jobsTypeDeletados.push_back(jobsType[j]);
                    jobsType.erase(jobsType.begin() + j);
                    
                }
            }
            // UPDATE NUMBER OF JOBS 
            numberJobs = jobsType.size(); 
            jobsType.push_back(superJob);

        }
        indexTmp.clear();

    }

    // UPDATE NUMBER OF TOOLS
    set <int> allTools;
    for(int i = 0; i < jobsType.size(); i++){
        for(int j = 0; j < jobsType[i].JobTools.size(); j++){
            allTools.insert(jobsType[i].JobTools[j]);
        }
    }
    numberTools = allTools.size();

}
