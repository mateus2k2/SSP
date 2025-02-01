#ifndef GlobalVarsH
#define GlobalVarsH
#include <atomic>
#include <vector>
#include <list>
#include <set>
#include <thread>
#include <map>

using namespace std;

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// TIPOS DA DADOS
// ------------------------------------------------------------------------------------------------------------------------------------------------------

typedef struct{
    int indexToolSet;
    vector<int> tools;
}ToolSet;

typedef struct{
    int indexJob;
    int indexOperation;
    int indexToolSet;
    int processingTime;
    bool priority;
    ToolSet toolSet;
    ToolSet toolSetNormalized;

    bool isGrouped;
    vector<int> processingTimes;
}Job;

struct JobTime {
    Job job;
    int machine;
    int start;
    int end;
    JobTime(Job job_, int machine_, int start_, int end_): job(job_), machine(machine_), start(start_), end(end_) {}
};

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// DEFINIÇÕES DO PLOBLEMA
// ------------------------------------------------------------------------------------------------------------------------------------------------------

#define COSTSWITCH         1
#define COSTSWITCHINSTANCE 10
#define COSTPRIORITY       30
#define PROFITYFINISHED    30
#define DAY                1440

#endif