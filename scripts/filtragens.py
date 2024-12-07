import pandas as pd

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
        jobsFilePath = f'{filePathJobs}'
        jobsDF = pd.read_csv(jobsFilePath, delimiter=';')
        jobsDict = jobsDF.to_dict(orient='records')
        
        for job in jobsDict:
            job['id'] = self.jobsId
            self.jobsId += 1
        
        return jobsDict

    def loadToolsets(self):
        toolSetsFilePath = f'{self.filePathToolsets}'
        toolSetsDF = pd.read_csv(toolSetsFilePath, header=None, delimiter=';')
        index = toolSetsDF.iloc[:, 0]
        toolSetsDF = toolSetsDF.drop(toolSetsDF.columns[0], axis=1)
        toolSetsDF = toolSetsDF.fillna('NaNPlaceholder')
        toolSetsDict = toolSetsDF.to_dict(orient='records')
        toolSetList = []
        for i in range(0, len(toolSetsDict)):
            toolSetList.append([])
            for tool in toolSetsDict[i].values():
                if tool != 'NaNPlaceholder':
                    toolSetList[i].append(int(tool))
        toolSetMap = {indexAtual : toolSetAtual for indexAtual, toolSetAtual in zip(index, toolSetList)}
        return toolSetMap

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
    # ----------------------------------------
    # file1000 Analisys
    # ----------------------------------------

    FiltersObj = Filters(['/home/mateus/WSL/IC/SSP/input/Processed/1000.csv'], '/home/mateus/WSL/IC/SSP/input/Processed/ToolSetInt.csv')

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
    
main()