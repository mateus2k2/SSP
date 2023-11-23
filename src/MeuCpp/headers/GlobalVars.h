#ifndef GlobalVarsH
#include <atomic>
#include <vector>
#include <thread>

using namespace std;


// ------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIVEIS DADOS
// ------------------------------------------------------------------------------------------------------------------------------------------------------

extern vector<int> operation;		 // 		
extern vector<int> job;			     // 	
extern vector<int> toolSet;		     // 	
extern vector<int> processingTime;	 // 	
extern vector<vector<int>> toolSets; // 
extern vector<int> machine;		     // *		
extern vector<int> priority;		 // *	
extern int planingHorizon;  		 // * Horizonte de planejamento (Dias)
extern int unsupervised; 			 // * Horas nao supervisionadas (Horas em cada dia)

extern int numberMachines;		     // * Nuemro de maquinas
extern int numberTools;      	     //   Numero de Ferramentas diferentes 
extern int numberJobs; 	   	         //   Numero de Jobs
extern int capacityMagazine;         //   Capacidade do magazine das maquina
extern vector<vector<int>> JobTools; //   Vetor de jobs com cada ferramenta que ele usa
extern vector<vector<bool>> toolJob; //   Vetor de ferramentas com cada job que ela pode fazer

extern unsigned int sum;   	         //   auxiliary of GPCA

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIVEIS GLOBAIS
// ------------------------------------------------------------------------------------------------------------------------------------------------------

extern int meio; 			   // auxiliary of KTNS PAR
extern vector<int> neigh; 	   // auxiliary of KTNS PAR
extern atomic_bool fase; 	   // auxiliary of KTNS PAR
extern atomic_bool endD;	   // auxiliary of KTNS PAR
extern atomic_bool stop;	   // auxiliary of KTNS PAR
extern vector<bool> magazineD; // auxiliary of KTNS PAR
extern int evalSolD;		   // auxiliary of KTNS PAR
extern thread tKT;			   // auxiliary of GPCA PAR
extern int last_fullD;		   // auxiliary of GPCA PAR
extern vector<int> last_seenD; // auxiliary of GPCA PAR
extern vector<vector<int>> MD; // auxiliary of GPCA PAR
extern int pipes_countD;	   // auxiliary of GPCA PAR

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// DEFINIÇÕES DO PLOBLEMA
// ------------------------------------------------------------------------------------------------------------------------------------------------------

#define COSTSWITCH 1
#define COSTSWITCHINSTANCE 10
#define COSTPRIORITY 30
#define PROFITYPRIORITY 30

#endif