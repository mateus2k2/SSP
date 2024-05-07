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
    int indexJob;
    int indexOperation;
    int indexToolSet;
    int processingTime;
    bool priority;
    vector<int> JobTools;  

}Job;


typedef struct{
    int indexToolSet;
    vector<int> tools;

}ToolSet;


typedef struct{
    int indexSuperToolSet;
    int processingTimeSum;
    int prioritySum;
    list<int> originalJobs;

}SuperJob;


typedef struct{
    int indexOriginalToolSet;
    list<int> originalToolSets;

}SuperToolSet;


// ------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIVEIS PARA CARREGAR DADOS
// ------------------------------------------------------------------------------------------------------------------------------------------------------

extern map<int, vector<int>>  mapToolSets; // Vetor com cada ToolSet
extern vector<int> JobToolsIndex;          // Vetor de jobs com cada ferramenta que ele usa
extern vector<vector<int>> JobTools;       // Vetor de jobs com cada ferramenta que ele usa

extern vector<int> operation;		       // Aperation Vector		
extern vector<int> job;			           // Job Vector	
extern vector<int> processingTime;	       // Processing Time de cada Vector	
extern vector<int> priority;	      

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIVEIS RESOLVER O PROBLEMA
// ------------------------------------------------------------------------------------------------------------------------------------------------------

extern int planingHorizon;  		 // Horizonte de planejamento (Dias)
extern int unsupervised; 			 // Horas nao supervisionadas (Horas em cada dia)
extern int numberMachines;		     // Nuemro de maquinas
extern int numberTools;      	     // Numero de Ferramentas diferentes 
extern int numberJobs; 	   	         // Numero de Jobs
extern int capacityMagazine;         // Capacidade do magazine das maquina

extern vector<Job> originalJobs;
extern vector<ToolSet> originalToolSets;

extern vector<SuperJob> superJobs;
extern vector<SuperToolSet> superToolSet;

extern set<pair<int,int>> priorityIndex;

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// DEFINIÇÕES DO PLOBLEMA
// ------------------------------------------------------------------------------------------------------------------------------------------------------

#define COSTSWITCH         1
#define COSTSWITCHINSTANCE 10
#define COSTPRIORITY       30
#define PROFITYPRIORITY    30

// #define TIMESCALE          24
#define TIMESCALE          1440

#define DEBUG


#endif