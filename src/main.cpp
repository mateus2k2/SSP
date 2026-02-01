#include <atomic>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <chrono>


#include "headers/GlobalVars.h"
#include "headers/SSP.h"

#ifdef FMT
#include <fmt/core.h>
#include <fmt/ranges.h>
#endif

#include "ExecTime.h"
#include "PT.h"

using namespace std;

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// MAIN
// ------------------------------------------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    //pt varibles
	float tempIni = 0.01;
	float tempfim = 2.0;
	int tempN = 10;
	int MCL = 0;
	int PTL = 2;
    int passoGatilho = 0;	
	int tempUp = 50;
	int tempD = 1;
	int uType = 0;
	int thN = thread::hardware_concurrency();
	int initSolType = 0;
	int ptlTempUpProportion = 1;
	vector<string> arguments(argv + 1, argv + argc);	

    int COSTSWITCH = 1;
    int COSTSWITCHINSTANCE = 10;
    int COSTPRIORITY = 30;
    int PROFITYFINISHED = 30;

    //problem varibles
	// int capacityMagazine       = 8;
    // int numberMachines         = 2;
    // int planingHorizon         = 2;
    // int unsupervised           = 0.5*DAY;
	int diferent_toolset_mode  = 0;
	int instance_report        = 0;

    // modelo
    int modelo = 0;
    int timeLimit = 0;

    // practitioner
    int practitioner = 0;
    int sequenceBy = 0;
	
	// Instance file name
	string filenameJobs = arguments[0];
	string filenameTools = arguments[1];
	string filenameoutput = arguments[2];
	
	// Read arguments
	for(unsigned int i=3; i<arguments.size(); i+=2)
	{
        if(arguments[i]== "--TEMP_FIM")
            tempfim = stof(arguments[i+1]);
        else if(arguments[i]== "--TEMP_INIT")
            tempIni = stof(arguments[i+1]);
        else if(arguments[i]== "--N_REPLICAS")
            tempN = stoi(arguments[i+1]);
        else if(arguments[i]== "--MCL")
            MCL  = stoi(arguments[i+1]);
        else if(arguments[i]== "--PTL")
            PTL = stoi(arguments[i+1]);
        else if(arguments[i]== "--TEMP_DIST")
            tempD = stoi(arguments[i+1]);
        else if(arguments[i]== "--TYPE_UPDATE")
            uType = stoi(arguments[i+1]);
        else if(arguments[i]== "--TEMP_UPDATE")
            tempUp = stoi(arguments[i+1]);
        else if(arguments[i]== "--THREAD_USED")
            thN = stoi(arguments[i+1]);
		else if(arguments[i]== "--INIT_SOL_TYPE")
			initSolType = stoi(arguments[i+1]);
		else if(arguments[i]== "--PTL_TEMP_UPDATE_PROPORTION")
			ptlTempUpProportion = stoi(arguments[i+1]);
		// else if(arguments[i]== "--CAPACITY")
		// 	capacityMagazine = stoi(arguments[i+1]);
		// else if(arguments[i]== "--MACHINES")
		// 	numberMachines = stoi(arguments[i+1]);
		// else if(arguments[i]== "--DAYS")
		// 	planingHorizon = stoi(arguments[i+1]);
		// else if(arguments[i]== "--UNSUPERVISED_MINUTS")
		// 	unsupervised = stoi(arguments[i+1]);
		else if(arguments[i]== "--INSTANCE_REPORT")
			instance_report = stoi(arguments[i+1]);
		else if(arguments[i]== "--DIFERENT_TOOLSETS_MODE")
			diferent_toolset_mode = stoi(arguments[i+1]);
        else if(arguments[i]== "--PASSO_GATILHO")
            passoGatilho = stoi(arguments[i+1]);
        else if(arguments[i]== "--MODELO")
            modelo = stoi(arguments[i+1]);
        else if(arguments[i]== "--TIME_LIMIT")
            timeLimit = stoi(arguments[i+1]);
        else if(arguments[i]== "--PRACTITIONER")
            practitioner = stoi(arguments[i+1]);
        else if(arguments[i]== "--SEQUENCE_BY")
            sequenceBy = stoi(arguments[i+1]);

        else if(arguments[i]== "--COSTSWITCH")
            COSTSWITCH = stoi(arguments[i+1]);
        else if(arguments[i]== "--COSTSWITCHINSTANCE")
            COSTSWITCHINSTANCE = stoi(arguments[i+1]);
        else if(arguments[i]== "--COSTPRIORITY")
            COSTPRIORITY = stoi(arguments[i+1]);
        else if(arguments[i]== "--PROFITYFINISHED")
            PROFITYFINISHED = stoi(arguments[i+1]);
            
    }
	tempUp = PTL/ptlTempUpProportion;

	SSP* prob = new SSP(filenameJobs,filenameTools,diferent_toolset_mode,COSTSWITCH, COSTSWITCHINSTANCE, COSTPRIORITY, PROFITYFINISHED);

    // ------------------------------------------------------------------------------
    // DATA LOADIDNG AND PRE PROCESSING
    // ------------------------------------------------------------------------------

    fstream solutionReportFile;
    solutionReportFile.open(filenameoutput, ios::out);
    if (!solutionReportFile.is_open() && !modelo) {
        cerr << "Error: Could not open solution report file: " << filenameoutput << endl;
        exit(1);
    }
    if (instance_report) prob->printDataReport();
    prob->groupJobs();
    
    // ------------------------------------------------------------------------------
    // MODELO
    // ------------------------------------------------------------------------------

    if(modelo){ 
        prob->modelo(filenameoutput, timeLimit);
        return 0;
    }

    if(practitioner){
        // cout << "Practitioner Mode" << endl;
        prob->practitioner(solutionReportFile, sequenceBy);
        return 0;
    }

    // ------------------------------------------------------------------------------
    // TEST
    // ------------------------------------------------------------------------------

    // prob->setParans(uType, initSolType);
    // solSSP solTeste = prob->randPriority();
    // double teste = prob->evaluate(solTeste);
    // cout << "Avaliação da solução teste: " << teste << endl;
    // double testeRetport = prob->evaluateReport(solTeste, solutionReportFile);
    // cout << "Avaliação da solução teste report: " << testeRetport << endl;
    // solutionReportFile << "Final Solution: " << testeRetport << endl;
    // prob->printDataReport();
    // return 0;
    
    // prob->setParans(uType, initSolType);
    // solSSP solTeste = prob->randPriority();
    // cout << "Initial Solution: " << prob->evaluate(solTeste) << endl;
    // prob->oneBlockGrouping(solTeste);
    // return 0;

    // prob->setParans(uType, initSolType);
    // double acc = 0.0;
    // for (int i = 0; i < 100; ++i) {
    //     solSSP solTeste = prob->randPriority();

    //     auto start = std::chrono::high_resolution_clock::now();
    //     double resultado = prob->evaluate(solTeste);
    //     auto end = std::chrono::high_resolution_clock::now();

    //     std::chrono::duration<double, std::micro> elapsed = end - start;
    //     acc += elapsed.count();
    // }
    // std::cout << "Average time for 100 random solutions: " << acc / 100.0 << " μs" << std::endl;
    // return 0;
    
    // solutionReportFile << "Final Solution: " << 0 << endl;
    // solutionReportFile << "Time: " << 0 << endl;
    // solutionReportFile << "PTL: " << 0 << endl;
    // solutionReportFile << "MCMC: " << 0 << endl;
    // solutionReportFile << "Best Initial: " << 0 << endl;
    // solutionReportFile << "Mean Initial: " << 0 << endl;
    // return 0;

    // PT<solSSP> algoTeste(tempIni, tempfim, tempN, MCL, PTL, passoGatilho, tempD, uType, tempUp);
    // solTeste = prob->neighbor(solTeste);
    // double eval1 = prob->evaluate(solTeste);
    // cout << "Avalicação do KTNS : " << eval1 << endl;
    // double eval2 = prob->evaluateReportKTNS(solTeste, filenameJobs, filenameTools, solutionReportFile);
    // cout << "Avalicação do KTNS Report : " << eval2  << endl;
    // return 0;

    // ------------------------------------------------------------------------------
    // SSP
    // ------------------------------------------------------------------------------

	prob->setParans(uType, initSolType);
    PT<solSSP> algo(tempIni, tempfim, tempN, MCL, PTL, passoGatilho, tempD, uType, tempUp);
    ExecTime et;
    solSSP sol = algo.start(thN, prob);
    solSSP finalSolution = sol; 

    double cost = prob->evaluateReport(finalSolution, solutionReportFile);

    solSSP initFromBest = algo.getInitFromBest();
    vector<solSSP> initAll = algo.getInitAll();

    double meanInitial = 0; 
    std::sort(initAll.begin(), initAll.end(), [](auto a, auto b) { return a.evalSol < b.evalSol; });
    for (size_t i = 0; i < initAll.size(); i++) meanInitial += initAll[i].evalSol;
    meanInitial = meanInitial/initAll.size();

    solutionReportFile << "Final Solution: " << cost << endl;
    solutionReportFile << "Time: " << et.getTimeMs() << endl;
    solutionReportFile << "PTL: " << sol.ptl << endl; //PTL onde a melhor solucao foi gerada
    solutionReportFile << "MCMC: " << sol.mcmc << endl; //Index da cadeia de markove onde a melhor solucao foi gerada
    solutionReportFile << "Best Initial: " << -initAll[0].evalSol << endl;
    solutionReportFile << "Mean Initial: " << -meanInitial << endl;

    solutionReportFile.close();

    // cout << "Final Solution: " << cost << endl;
    // cout << "Time: " << et.getTimeMs() << endl;

    // convert ms to min
    double timeInMin = et.getTimeMs() / 60000.0;
    int minutes = static_cast<int>(std::round(timeInMin));
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << minutes;
    std::string padded = oss.str();
    cout << cost << "." << padded << endl;

    // fstream solutionReportFileInit;
    // solutionReportFileInit.open("/home/mateus/WSL/IC/SSP/output/Exemplo/exemploInit.txt", ios::out);
    // prob->evaluateReportKTNS(initAll[0], filenameJobs, filenameTools, solutionReportFileInit);

    return 0;
}