#ifndef CostH

#include <vector>
#include <string>

using namespace std;

unsigned int KTNS(vector<int> s);
unsigned int KTNSReport(vector<int> s, fstream& solutionReportFile);

unsigned int costReport(vector<int> s, vector<int> machine, fstream& solutionReportFile);
unsigned int cost(vector<int> s);

#endif