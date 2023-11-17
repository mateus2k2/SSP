#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

vector<int> operation;
vector<int> job;
vector<int> toolSet;
vector<int> processingTime;
vector<vector<int>> toolSets;

int numberJobs, numberTools, capacityMagazine;	
vector<vector<bool>>  toolJob;
vector<vector<int>>  JobTools; 
unsigned int sum = 0;


void loadInstance(string filename){
	
	// Variaveis locais
	string line; 
    ifstream ifs;
	int i=0;
	int j=0;
	
	// Abre o arquivo com a instância
	ifs.open(filename);
			
	// Ler o arquivo
	if ( ifs.is_open()){
		getline(ifs,line);
		numberJobs = stoi(line); // Recupera a quantidade de tarefas

		getline(ifs,line);
		numberTools = stoi(line); // Recupera a quantidade de Ferramentas
		
		getline(ifs,line);
		capacityMagazine = stoi(line); // Recupera a capacidade do magazine
		
		// Redimensiona as variaveis 		
		toolJob.assign(numberTools, vector<bool>(numberJobs,false));
		JobTools.resize(numberJobs);
			
		// Preenche as variaveis	
		while(getline(ifs,line)){
			
			stringstream ss(line);
			while ( getline (ss ,line, ' ')){
				toolJob[i][j] = stoi(line);
				if(toolJob[i][j]){
					 JobTools[j].push_back(i);
					 ++sum;
				}
			j++;
			}
		j=0;
		i++;
		}
	// Fecha o arquivo	
	ifs.close();
	}else{
		cout << "Could not open the file! \n";
	}
}

int laodRealInstance(string filename){
	// Open the CSV file
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    // Read each line from the file
    string line;
	getline(file, line);
    while (getline(file, line)) {
        stringstream ss(line);
        string value;

        // Read each value separated by semicolon
        getline(ss, value, ';');
        operation.push_back(stoi(value));

        getline(ss, value, ';');
        job.push_back(stoi(value));

        getline(ss, value, ';');
        toolSet.push_back(stoi(value));

        getline(ss, value, '\n'); // Read until the end of the line
        processingTime.push_back(stoi(value));
    }

    // Close the file
    file.close();

    return 0;
}

int laodToolSet(string filename) {
    // Open the CSV file
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    // Read each line from the file
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string value;
        vector<int> lineData;

        // Read each value separated by semicolon
		getline(ss, value, ';');
        while (getline(ss, value, ';')) {
            // Convert each value to an integer and add it to the vector
			if (!value.empty()){
                lineData.push_back(stoi(value));
			}
			else{
				break;
			}
        }

        // Add the vector for the current line to the main vector
        toolSets.push_back(lineData);
    }

    // Close the file
    file.close();

    return 0;
}

int main(){
    
	// ------------------------------------------------------------------------------------------------------------------------

    // loadInstance("/home/mateus/WSL/IC/data/Example.txt");
    
    // cout << "numberJobs: " << numberJobs << "\n";
    // cout << "numberTools: " << numberTools << "\n";
    // cout << "capacityMagazine: " << capacityMagazine << "\n";
    // cout << "sum: " << sum << "\n";
    
    // for(unsigned int i = 0; i < numberJobs; ++i){
    //     cout << "JobTools[" << i << "]: ";
    //     for (auto t = JobTools[i].begin(); t != JobTools[i].end(); ++t){
    //         cout << *t << " ";
    //     }
    //     cout << "\n";
    // }
    
    // for(unsigned int i = 0; i < numberTools; ++i){
    //     cout << "toolJob[" << i << "]: ";
    //     for (auto t = toolJob[i].begin(); t != toolJob[i].end(); ++t){
    //         cout << *t << " ";
    //     }
    //     cout << "\n";
    // }

	// ------------------------------------------------------------------------------------------------------------------------

	// laodRealInstance("/home/mateus/WSL/IC/data/Data Jobs 250.csv");

	// // Print the vectors
    // cout << "Operation: ";
    // for (const auto &op : operation) {
    //     cout << op << " ";
    // }
    // cout << endl;

    // cout << "Job: ";
    // for (const auto &j : job) {
    //     cout << j << " ";
    // }
    // cout << endl;

    // cout << "ToolSet: ";
    // for (const auto &ts : toolSet) {
    //     cout << ts << " ";
    // }
    // cout << endl;

    // cout << "Processing Time: ";
    // for (const auto &pt : processingTime) {
    //     cout << pt << " ";
    // }
    // cout << endl;

	// ------------------------------------------------------------------------------------------------------------------------

	// laodToolSet("/home/mateus/WSL/IC/data/Data Tool Set Compositions.csv");
	
	// print toolSets
	// for (const auto &line : toolSets) {
	//     for (const auto &value : line) {
	//         cout << value << " ";
	//     }
	//     cout << endl;
	// }

	// return 0;

	// ------------------------------------------------------------------------------------------------------------------------
}

/* 

/home/mateus/WSL/IC/dataRaw/KTNS/Instances/Catanzaro/Tabela1/datA1
10
10
4
0 0 1 1 0 1 0 0 1 1 
0 1 0 0 0 0 0 0 0 1 
0 1 0 0 0 1 0 0 0 0 
0 0 0 0 0 0 0 0 1 1 
0 0 0 1 1 0 1 1 1 0 
1 0 1 0 0 1 1 0 0 0 
1 0 0 0 0 0 0 1 0 0 
0 0 0 0 1 1 1 1 1 0 
0 0 1 1 0 0 1 0 0 0 
0 0 0 0 1 0 0 0 0 1 


/home/mateus/WSL/IC/dataRaw/KTNS/Instances/Catanzaro/Tabela1/datB9
15
20
6
0 0 0 1 0 0 0 1 0 1 0 0 1 0 0 
0 0 1 1 0 0 0 0 0 0 1 0 1 0 0 
0 0 0 0 0 1 0 1 0 0 0 0 0 0 0 
0 0 0 1 0 0 0 0 0 1 0 0 0 0 0 
0 0 0 0 0 1 0 0 1 0 1 0 0 0 0 
1 0 0 0 0 1 0 0 0 0 1 0 0 0 1 
0 0 0 1 0 0 0 0 1 1 0 0 0 0 1 
0 0 0 0 1 1 0 1 0 0 1 0 0 0 1 
0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 
1 0 0 0 0 0 0 0 0 0 0 0 1 0 1 
0 0 0 0 1 0 0 0 0 0 0 0 1 1 0 
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
0 0 0 1 0 0 0 0 0 0 0 1 0 0 0 
0 0 0 1 0 0 1 0 0 0 0 0 0 0 0 
0 0 0 0 1 0 0 1 0 1 1 0 1 1 0 
0 1 0 0 1 0 1 0 0 0 0 1 0 1 0 
0 0 0 0 0 0 0 1 0 0 0 0 0 1 0 
0 0 0 0 0 0 0 1 0 0 0 0 0 1 0 
1 1 0 0 0 1 0 0 0 0 1 0 0 0 0 
0 0 0 0 1 0 0 0 0 0 0 0 0 0 1
*/