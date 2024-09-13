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
}Job;

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// DEFINIÇÕES DO PLOBLEMA
// ------------------------------------------------------------------------------------------------------------------------------------------------------

#define COSTSWITCH         1
#define COSTSWITCHINSTANCE 10
#define COSTPRIORITY       30
#define PROFITYFINISHED    30

#define DAY                1440

#define PRINTS

#endif