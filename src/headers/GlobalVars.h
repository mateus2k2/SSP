#ifndef GlobalVarsH
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
// VARIVEIS RESOLVER O PROBLEMA
// ------------------------------------------------------------------------------------------------------------------------------------------------------

extern fstream solutionReportFile;

extern int planingHorizon;  		 // Horizonte de planejamento (Dias)
extern int unsupervised; 			 // Horas nao supervisionadas (Horas em cada dia)
extern int numberMachines;		     // Nuemro de maquinas
extern int numberTools;      	     // Numero de Ferramentas diferentes 
extern int numberJobs; 	   	         // Numero de Jobs
extern int capacityMagazine;         // Capacidade do magazine das maquina

extern vector<Job> originalJobs;
extern map<int, ToolSet> originalToolSets;

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// DEFINIÇÕES DO PLOBLEMA
// ------------------------------------------------------------------------------------------------------------------------------------------------------

#define COSTSWITCH         1
#define COSTSWITCHINSTANCE 10
#define COSTPRIORITY       30
#define PROFITYPRIORITY    30

#define DAY                1440

#define PRINTS

#endif