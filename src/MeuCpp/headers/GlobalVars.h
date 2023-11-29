#ifndef GlobalVarsH
#include <atomic>
#include <vector>
#include <thread>
#include <map>

using namespace std;


// ------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIVEIS DADOS
// ------------------------------------------------------------------------------------------------------------------------------------------------------

extern map<int, vector<int>>  mapToolSets; // Vetor com cada ToolSet

extern vector<int> operation;		 // Aperation Vector		
extern vector<int> job;			     // Job Vector	
extern vector<int> processingTime;	 // Processing Time de cada Vector	
extern vector<int> machine;		     // Maquina que cada job vai ser feito (Index comeca no 0)	
extern vector<int> priority;		 // Prioridade de cada job (0 ou 1)
extern int planingHorizon;  		 // Horizonte de planejamento (Dias)
extern int unsupervised; 			 // Horas nao supervisionadas (Horas em cada dia)

extern int numberMachines;		     // Nuemro de maquinas
extern int numberTools;      	     // Numero de Ferramentas diferentes 
extern int numberJobs; 	   	         // Numero de Jobs
extern int capacityMagazine;         // Capacidade do magazine das maquina
extern vector<int> JobToolsIndex;    // Vetor de jobs com cada ferramenta que ele usa
extern vector<vector<int>> JobTools; // Vetor de jobs com cada ferramenta que ele usa
extern vector<vector<bool>> toolJob; // Vetor de ferramentas com cada job que ela pode fazer

extern unsigned int sum;   	         // auxiliary

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// DEFINIÇÕES DO PLOBLEMA
// ------------------------------------------------------------------------------------------------------------------------------------------------------

#define COSTSWITCH 1
#define COSTSWITCHINSTANCE 10
#define COSTPRIORITY 30
#define PROFITYPRIORITY 30

// #define DEBUG

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// TIPOS DA DADOS
// ------------------------------------------------------------------------------------------------------------------------------------------------------

typedef struct{
    int indexJob;
    int indexOperation;
    int indexToolSet;
    int processingTime;
    bool priority;
    int indexMachine;
    
    vector<int> JobTools;  

    bool superJob;
    vector<int> originalJobs;
} Job;


typedef struct{
    int indexToolSet;
    vector<int> tools;

    bool superToolSet;
    vector<int> originalToolSets;
} ToolSet;


#endif