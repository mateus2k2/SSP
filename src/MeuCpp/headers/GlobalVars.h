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
extern int currantHour;  		     // Hora Atual 
extern int currantDay; 			     // Dia Atual

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