#ifndef CostH

#include <vector>
#include <string>

using namespace std;

unsigned int GPCA(vector<int> s);	
unsigned int GPCAPar(vector<int> s);
void threadRunGPCA();
unsigned int GPCAOut(vector<int> s);

unsigned int KTNS(vector<int> s);
unsigned int KTNSReportIfDef(vector<int> s);
unsigned int KTNSReport(vector<int> s);

unsigned int printSolutionReport(vector<int> s);
unsigned int printSolutionReportIfDef(vector<int> sol);
unsigned int cost(vector<int> s);

#endif