import pandas as pd

def dumpCSV(dictData, path):
    df = pd.DataFrame.from_dict(dictData)
    df.to_csv(path, sep=';', index=False, header=False)

class ToolSet:
    def __init__(self, filePath: str):
        self.toolSets = []
        
        self.used = []
        self.unused = []

        self.largerThen80, self.smallerThen80 = self.cleanBiggerThen(80)
        self.smallerThen1, self.biggerThen1 = self.cleanSmallerThen(1)
        self.notSubSets = self.cleanBySubSets()
        self.clean = self.sumToolSets([self.notSubSets, self.biggerThen1, self.smallerThen80])

        self.filePath = filePath
        self.load()

    def load(self):
        file_path = f'/home/mateus/WSL/IC/SSP/input/{self.filePath}'
        
        # Read CSV into DataFrame
        tool_sets_df = pd.read_csv(file_path, header=None, delimiter=';')

        # Extract index and drop first column
        index_column = tool_sets_df.iloc[:, 0]
        tool_sets_df = tool_sets_df.drop(columns=[0])

        # Replace NaN values with a placeholder
        tool_sets_df = tool_sets_df.fillna('NaNPlaceholder')

        # Convert DataFrame to list of lists while filtering out placeholders
        tool_sets = tool_sets_df.map(lambda x: int(x) if x != 'NaNPlaceholder' else 'NaNPlaceholder')
        tool_sets_list = tool_sets.apply(lambda row: [val for val in row if val != 'NaNPlaceholder'], axis=1).tolist()

        # Map index to the corresponding tool set
        self.toolSets = dict(zip(index_column, tool_sets_list))

    def sumToolSets(self, setsList):
        newList = []
        newListIndex = []

        for set in setsList:
            for item in set:
                if item not in newListIndex:
                    newList.append(set[item])
                    newListIndex.append(item)

    def cleanBiggerThen(self, maxSize: int = 80):
        fit = {}
        unFit = {}
        for toolSet in self.toolSets:
            if len(self.toolSets[toolSet]) > maxSize:
                fit[toolSet] = self.toolSets[toolSet]
            else:
                unFit[toolSet] = self.toolSets[toolSet]
        return fit, unFit
    
    def cleanSmallerThen(self, minSize: int = 0):
        fit = {}
        unFit = {}
        for toolSet in self.toolSets:
            if len(self.toolSets[toolSet]) > minSize:
                fit[toolSet] = self.toolSets[toolSet]
            else:
                unFit[toolSet] = self.toolSets[toolSet]
        return fit, unFit

    def cleanBySubSets(self):
        removedDuplicates = []
        
        setFound = set()
        for toolSet in self.toolSets:
            setFound.add(toolSet)
        
        removedDuplicates.sort(key=lambda x: len(self.toolSets[x]), reverse=True)
        deletedSubSets = []
        FilteredList = removedDuplicates.copy()
        
        # Remove subconjuntos
        for i, toolSet in enumerate(removedDuplicates):
            jobSet = set(self.toolSets[toolSet])
            for j in range(i+1, len(removedDuplicates)):
                jobsInsideSet = set(self.toolSets[removedDuplicates[j]])
                if jobsInsideSet.issubset(jobSet) and removedDuplicates[j] not in deletedSubSets:
                    deletedSubSets.append(removedDuplicates[j])
                    FilteredList.remove(removedDuplicates[j])
        
        return FilteredList


    def __str__(self):
        return f"{self.toolSets}"

class Job:
    def __init__(self, filePath: str = '', toolSets: ToolSet = None):
        self.jobs = []
        self.filePath = filePath
        self.toolSets = toolSets
        self.load()
        self.removeUnsuedToolSets()

    def load(self):
        jobsFilePath = f'/home/mateus/WSL/IC/SSP/input/{self.filePath}'
        jobsDF = pd.read_csv(jobsFilePath, delimiter=';')
        jobsDict = jobsDF.to_dict(orient='records')
        self.jobs = jobsDict

        for job in self.jobs:
            job['toolSet'] = self.toolSets.toolSets[job['ToolSet']]

    def clean(self, toolSetToRemove):
        newList = {}
        newListIndex = {}
        indexToRemove = toolSetToRemove.keys()
        for job in self.jobs:
            if not (job['ToolSet'] in indexToRemove):
                newList[job['ToolSet']] = job
                newListIndex[job['ToolSet']] = job['ToolSet']
        return newList

    def removeUnsuedToolSets(self):
        usedToolSets = {}
        unusedToolSets = {}
        for toolSet in self.toolSets.toolSets:
            if toolSet in [job['ToolSet'] for job in self.jobs]:
                usedToolSets[toolSet] = self.toolSets.toolSets[toolSet]
            else:
                unusedToolSets[toolSet] = self.toolSets.toolSets[toolSet]

        self.toolSets.used = usedToolSets
        self.toolSets.unused = unusedToolSets

        return {
            'used': usedToolSets,
            'unused': unusedToolSets
        }

    def __str__(self):
        return f'{self.jobs}'
    

newToolSet = ToolSet('ExemploArtigo/ToolSets.csv')
print(newToolSet)

newJob = Job('ExemploArtigo/Jobs.csv', newToolSet)
obj = newJob.removeUnsuedToolSets()
print(obj)