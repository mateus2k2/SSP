from pathlib import Path

from uteis.loadData import loadJobs as _loadJobs, loadToolSet as _loadToolSet

REPO_ROOT = Path(__file__).resolve().parent.parent


class Filters:
    def __init__(self, filePathListJobs: list, filePathToolsets: str):
        self.filePathListJobs = filePathListJobs
        self.filePathToolsets = filePathToolsets
        
        self.jobsId = 0
        
        self.toolSets = self.loadToolsets()
        self.jobs = [job for filePathJobs in filePathListJobs for job in self.loadJobs(filePathJobs)]
        self.toolSetsUsed, self.toolSetsNotUsed = self.getToolsetsUsedAndNotUsed(self.jobs) 
    
    # ----------------------------------------
    # uteis
    # ----------------------------------------
    
    def removeIdFromJobs(self):
        # return a copy of the jobs list without the id
        jobsCopy = self.jobs.copy()
        for jobsCopy in self.jobs:
            del jobsCopy['id']
        return jobsCopy
    
    def getToolsetsUsedAndNotUsed(self, jobs):
        toolsetsUsed = {}
        toolsetsNotUsed = {}
        
        for job in jobs: toolsetsUsed[job['ToolSet']] = self.toolSets[job['ToolSet']]
        for toolset in self.toolSets.keys():
            if toolset not in toolsetsUsed.keys():
                toolsetsNotUsed[toolset] = self.toolSets[toolset]

        return toolsetsUsed, toolsetsNotUsed

    def concatToolsetsDics(self, toolsetsLists):
        # given a list of toolsets dictionaries, return a dictionary with all the toolsets
        toolsets = {}
        for toolsetsList in toolsetsLists:
            for toolset in toolsetsList.keys():
                toolsets[toolset] = toolsetsList[toolset]
        return toolsets

    # ----------------------------------------
    # laoding and unloading
    # ----------------------------------------
    
    def loadJobs(self, filePathJobs):
        # Delegates to uteis.loadData (was a copy-pasted reimplementation).
        jobsDict = _loadJobs(filePathJobs)

        for job in jobsDict:
            job['id'] = self.jobsId
            self.jobsId += 1

        return jobsDict

    def loadToolsets(self):
        # Delegates to uteis.loadData (was a copy-pasted reimplementation).
        return _loadToolSet(self.filePathToolsets)

    def saveListToFile(self, listToSave, filePath):
        # save a list to a file
        pass
    
    # ----------------------------------------
    # filtragem por tamanho
    # ----------------------------------------
    
    def getJobsWithToolsetsLargarThen(self, jobsToFilter, toolsetLength):
        filteredJobs = []
        filteredToolsets = {}
        for jobToFilter in jobsToFilter:
            toolset = self.toolSets[jobToFilter['ToolSet']]
            if len(toolset) > toolsetLength:
                filteredJobs.append(jobToFilter)
                filteredToolsets[jobToFilter['ToolSet']] = toolset
        return filteredJobs, filteredToolsets
    
    def getJobsWithToolsetsShorterThen(self, jobsToFilter, toolsetLength):
        filteredJobs = []
        filteredToolsets = {}
        for jobToFilter in jobsToFilter:
            toolset = self.toolSets[jobToFilter['ToolSet']]
            if len(toolset) < toolsetLength:
                filteredJobs.append(jobToFilter)
                filteredToolsets[jobToFilter['ToolSet']] = toolset
        return filteredJobs, filteredToolsets
    
    def getToolsetsLargarThen(self, jobsToFilter, toolsetLength):
        filteredToolsets = {}
        for jobToFilter in jobsToFilter:
            toolset = self.toolSets[jobToFilter['ToolSet']]
            if len(toolset) >= toolsetLength:
                filteredToolsets[jobToFilter['ToolSet']] = toolset
        return filteredToolsets
    
    def getToolsetsShorterThen(self, jobsToFilter, toolsetLength):
        filteredToolsets = {}
        for jobToFilter in jobsToFilter:
            toolset = self.toolSets[jobToFilter['ToolSet']]
            if len(toolset) <= toolsetLength:
                filteredToolsets[jobToFilter['ToolSet']] = toolset
        return filteredToolsets
    
    # ----------------------------------------
    # filtragem por subconjunto
    # ----------------------------------------
    
    def getToolsetsAndJobsSubset(self, jobsToFilter, toolsetsToFilter):
        # given a list of jobs and a list of toolsets, return the toolsets that are a subset of someone else, and the jobs that have these toolsets
        subsetJobs = []
        subsetToolsets = {}
        for jobToFilter in jobsToFilter:
            toolset = self.toolSets[jobToFilter['ToolSet']]
            for toolsetToFilter in toolsetsToFilter:
                currantToolset = self.toolSets[toolsetToFilter]
                if set(currantToolset).issubset(set(toolset)):
                    subsetJobs.append(jobToFilter)
                    subsetToolsets[jobToFilter['ToolSet']] = toolset
                    break
        return subsetJobs, subsetToolsets
    
    def getToolsetsSubset(self, jobsToFilter, toolsetsToFilter):
        # given a list of jobs and a list of toolsets, return the toolsets that are a subset of someone else, and the jobs that have these toolsets
        subsetJobs = []
        subsetToolsets = {}
        for jobToFilter in jobsToFilter:
            toolset = self.toolSets[jobToFilter['ToolSet']]
            for toolsetToFilter in toolsetsToFilter:
                if set(toolsetToFilter).issubset(set(toolset)):
                    subsetJobs.append(jobToFilter)
                    subsetToolsets[jobToFilter['ToolSet']] = toolset
                    break
        return subsetJobs, subsetToolsets
    
    # ----------------------------------------
    # filtragem
    # ----------------------------------------
    
    def filterByList(self, jobsToFilter, toolsetsToFilter):
        # given a list of jobs and a list of toolsets, return the jobs and toolsets that are not in the list
        filteredJobs = []
        filteredToolsets = []
        
        for job in self.jobs:
            filterIn = True            
            for jobToFilter in jobsToFilter:
                if job['id'] == jobToFilter['id']:
                    filterIn = False
                    break
            if filterIn:
                filteredJobs.append(job)
        
        for toolset in self.toolSets.keys():
            filterIn = True
            for toolsetToFilter in toolsetsToFilter.keys():
                if toolset == toolsetToFilter:
                    filterIn = False
                    break
            if filterIn:
                filteredToolsets.append(toolset)
                            
        return filteredJobs, filteredToolsets


def main():
    import argparse

    parser = argparse.ArgumentParser(
        description="Report jobs/toolsets that are oversize (>80 tools), undersize (<1), "
                    "or subsets of an oversize toolset -- candidates for filtering out.")
    parser.add_argument("jobs_files", nargs="*",
                         default=[str(REPO_ROOT / "input" / "Processed" / "1000.csv")],
                         help="Job CSV file(s) to analyze (default: input/Processed/1000.csv)")
    parser.add_argument("--toolsets", default=str(REPO_ROOT / "input" / "Processed" / "ToolSetInt.csv"),
                         help="Toolset CSV file (default: input/Processed/ToolSetInt.csv)")
    args = parser.parse_args()

    FiltersObj = Filters(args.jobs_files, args.toolsets)

    jobslargerThen80, toolsetsLargerThen80 = FiltersObj.getJobsWithToolsetsLargarThen(FiltersObj.jobs, 81)
    jobsSmallerThen1, toolsetsSmallerThen1 = FiltersObj.getJobsWithToolsetsShorterThen(FiltersObj.jobs, 0)
    jobsSubsets, toolsetsSubsets = FiltersObj.getToolsetsAndJobsSubset(FiltersObj.jobs, toolsetsLargerThen80)

    problematicJobs = jobslargerThen80 + jobsSmallerThen1 + jobsSubsets
    problematicToolsets = FiltersObj.concatToolsetsDics([toolsetsLargerThen80, toolsetsSmallerThen1, toolsetsSubsets])
    filtedJobs, filteredToolsets = FiltersObj.filterByList(problematicJobs, problematicToolsets)

    print(f'Usados Maiores que 80 | Jobs: {len(jobslargerThen80)} toolSets: {len(toolsetsLargerThen80)}')
    print(f'Usados Menores que 1 | Jobs: {len(jobsSmallerThen1)} toolSets: {len(toolsetsSmallerThen1)}')
    print(f'Subconjuntos | Jobs: {len(jobsSubsets)} toolSets: {len(toolsetsSubsets)}')
    print(f'Filtrados | Jobs: {len(filtedJobs)} toolSets: {len(filteredToolsets)}')


if __name__ == "__main__":
    main()