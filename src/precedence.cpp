#include "headers/SSP.h"

#ifdef DEBUG
#include <fmt/core.h>
#include <fmt/ranges.h>
#endif

solSSP SSP::ajustFinalSolution(solSSP sol) {
    cout << "TESTE" << jobsTime.size() << endl;
    
    for (size_t i = 0; i < jobsTime.size(); i++){
        cout << fmt::format("Job: {0}, Machine: {1}, Start: {2}, End: {3}, Release: {4}, Due: {5}", jobsTime[i].job.indexJob, jobsTime[i].machine, jobsTime[i].start, jobsTime[i].end, jobsTime[i].releaseDate, jobsTime[i].dueDate) << endl;
        cout << "\n" << endl;
    }

    return sol;
}