import pandas as pd

def loadJobs(number):
    jobsFilePath = f'/home/mateus/WSL/IC/data/{number}.csv'
    jobsDF = pd.read_csv(jobsFilePath, delimiter=';')
    jobsDict = jobsDF.to_dict(orient='records')
    
    return jobsDict


jobs = loadJobs(1000)

sumProcessingTime = 0
for i in range(0, 210):
    sumProcessingTime += jobs[i]['Processing Time']

# 7 * 24 * 60 = 10080
print(sumProcessingTime)

# /6 pq nao 6 maquinas
print(sumProcessingTime/6)