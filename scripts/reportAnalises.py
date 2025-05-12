# passar pelo validador
# quantas vezes as precedencia foi quebrada
# analisar os tempos e os resultados = formato facil de passar pra planilha

import sys
import os
from natsort import natsorted
import statistics

import uteis.reportParser as rp
import uteis.validador as vd
import uteis.loadData as ld

# ---------------------------------------------------------------------------------------------------
# VALIDADOR
# ---------------------------------------------------------------------------------------------------

def validarPasta(files):

    for report in files:
        print(f"---Validating {report}---")
    
        planejamento, machines, endInfo = rp.parseReport(report)
        toolSets = ld.loadToolSet(planejamento['toolSetFileName'])
        jobs = ld.loadJobs(planejamento['jobsFileName'])
        print(endInfo)

        vd.checkMagazine(machines, toolSets, jobs)
        print()
        vd.checkUnsupervisedSwitchs(machines, toolSets, jobs, planejamento)
        print()
        vd.checkSwitchs(machines, endInfo, toolSets, jobs)
        print()
        vd.checkUnfinishedJobs(machines, jobs)
        print()
        vd.checkOperations(machines, jobs)
        print()
        vd.checkProfit(machines, endInfo, jobs, planejamento)
        print()
        vd.newKTNS(machines, toolSets, jobs, planejamento)
        print()
        vd.checkMagazineSize(machines, toolSets, jobs)
        print()

def verificarPrecedencia(files):
    quantidadePrecedenciaQuebradaPorInstancia = [0 for _ in range(len(files))]

    for index, report in enumerate(files):
        planejamento, machines, endInfo = rp.parseReport(report)
        jobs = ld.loadJobs(planejamento['jobsFileName'])
        FimOperacao0 = {}

        for machine in machines:
            for estado in machine:
                job = estado['job']
                if estado['operation'] == 0: FimOperacao0[job] = estado['end']

        # print(FimOperacao0)

        for machine in machines:
            for estado in machine:
                job = estado['job']
                if estado['operation'] == 1: 
                    if not (job in FimOperacao0) or estado['start'] < FimOperacao0[job]: 
                        # print(f'Precedencia quebrada para a instancia {index + 1} no job {job}')
                        quantidadePrecedenciaQuebradaPorInstancia[index] += 1

    return quantidadePrecedenciaQuebradaPorInstancia

def verificarPrecedenciaAsSingleMachine(files):
    quantidadePrecedenciaQuebradaPorInstancia = [0 for _ in range(len(files))]

    for index, report in enumerate(files):
        planejamento, machines, endInfo = rp.parseReport(report)
        jobs = ld.loadJobs(planejamento['jobsFileName'])
        precedencia = [[] for _ in range(len(jobs))]

        for machine in machines:
            for estado in machine:
                job = estado['job']
                if estado['operation'] == 0: precedencia[job].append(0)
                if estado['operation'] == 1: 
                    if not precedencia[job]: 
                        print(f'Precedencia quebrada para a instancia {index + 1} no job {job}')
                        quantidadePrecedenciaQuebradaPorInstancia[index] += 1
                    else : precedencia[job].pop()
    
    return quantidadePrecedenciaQuebradaPorInstancia

def verificarPares(files):
    for index, report in enumerate(files):
        planejamento, machines, endInfo = rp.parseReport(report)
        jobs = ld.loadJobs(planejamento['jobsFileName'])

        for machine in machines:
            for estado in machine:
                # para cada job operation 0, vericicar se existe um operation 1 na list "jobs"
                # se existir, verificar se ele existe 
                job = estado['job']
                problemaDeOperation1 = False
                if estado['operation'] == 0: 
                    for jobOriginal in jobs:
                        if jobOriginal['Job'] == job and jobOriginal['Operation'] == 1:
                            # print(f'Job {Job} esta na solucao e tem operacao 1')
                            problemaDeOperation1 = True
                            # verificar se a operacao 1 esta na solucao
                            for machine in machines:
                                for estado in machine:
                                    if estado['job'] == job and estado['operation'] == 1:
                                        problemaDeOperation1 = False
                                        break
                if problemaDeOperation1:
                    print(f'Problema de Operation 1 para a instancia {index + 1} no job {job}')

    return 0

# ---------------------------------------------------------------------------------------------------
# ANALISES
# ---------------------------------------------------------------------------------------------------

def totalUnfinishedJobs(machines, planejamento):
    jobs = ld.loadJobs(planejamento['jobsFileName'])
    total = len(jobs)
    for machine in machines:
        total -= len(machine)
    return total

def getFileParans(file):
    # n=600,p=0.25,r=0.5,t=3096,v6.csv
    parans = file.split(',')
    n = int(parans[0].split('=')[1])
    p = float(parans[1].split('=')[1])
    r = float(parans[2].split('=')[1])
    t = int(parans[3].split('=')[1])
    return n, p, r, t

def tabelaResultadosPractitioner(files):
    modoPlanilha = False
    separador = '&'
    for index, report in enumerate(files):
        planejamento, machines, endInfo = rp.parseReport(report)
        totalUnfineshed = totalUnfinishedJobs(machines, planejamento)

        instancename = report.split('/')[-1]
        instancenameClear = instancename.split(",t=")[0]
        componentesDoNome = instancenameClear.split(',')
        totalTarefas = int(componentesDoNome[0].split('=')[1])
        taxaPrioridade = float(componentesDoNome[1].split('=')[1])
        taxaReentrancia = float(componentesDoNome[2].split('=')[1])

        timeInSeconds = endInfo['Time'] / 1000

        endPrint = ''
        if index == len(files) - 1: endPrint = ' \\\\ \\hline'
        else : endPrint = ' \\\\'
        outputTeste = ((
            f'{totalTarefas} {separador} ' # n
            f'{taxaPrioridade} {separador} ' # p
            f'{taxaReentrancia} {separador} ' # r   
            f'{endInfo["fineshedJobsCount"]:,.2f} {separador}'
            f'{endInfo["unfineshedPriorityCount"]:,.2f} {separador}'
            f'{totalUnfineshed:,.2f} {separador}'
            f'{endInfo["switchsInstances"]:,.2f} {separador}'
            f'{endInfo["switchs"]:,.2f} {separador}'
            f'{endInfo["finalSolution"]:,.2f}'
            # f'{timeInSeconds:,.2f}'
            f'{endPrint}'
        ).replace('.', ',').replace(',00', ''))
        if(modoPlanilha): outputTeste = outputTeste.replace('&', ';').replace('\\\\', '').replace('\\hline', '')
        print(outputTeste)
        if((index+1) % 3 == 0 and not(index == len(files) - 1) and not modoPlanilha): print("\\hline")

def tabelaResultadosModelo(files):
    modoPlanilha = False
    separador = '&'
    for index, report in enumerate(files):
        planejamento, machines, endInfo = rp.parseReport(report)
        totalUnfineshed = totalUnfinishedJobs(machines, planejamento)

        instancename = report.split('/')[-1]
        instancenameClear = instancename.split(",t=")[0]
        componentesDoNome = instancenameClear.split(',')
        totalTarefas = int(componentesDoNome[0].split('=')[1])
        taxaPrioridade = float(componentesDoNome[1].split('=')[1])
        taxaReentrancia = float(componentesDoNome[2].split('=')[1])

        endPrint = ''
        if index == len(files) - 1: endPrint = ' \\\\ \\hline'
        else : endPrint = ' \\\\'
        outputTeste = ((
            f'{totalTarefas} {separador} ' # n
            f'{taxaPrioridade} {separador} ' # p
            f'{taxaReentrancia} {separador} ' # r
            f'{endInfo["fineshedJobsCount"]:,.2f} {separador}'
            f'{endInfo["unfineshedPriorityCount"]:,.2f} {separador}'
            f'{totalUnfineshed:,.2f} {separador}'
            f'{endInfo["switchsInstances"]:,.2f} {separador}'
            f'{endInfo["switchs"]:,.2f} {separador}'
            f'{endInfo["bestBound"]:,.2f} {separador}'
            f'{endInfo["finalSolution"]:,.2f} {separador}'
            f'{endInfo['Time']}'
            f'{endPrint}'
        ).replace('.', ',').replace(',00', ''))
        if(modoPlanilha): outputTeste = outputTeste.replace('&', ';').replace('\\\\', '').replace('\\hline', '')
        print(outputTeste)
        if((index+1) % 3 == 0 and not(index == len(files) - 1) and not modoPlanilha): print("\\hline")

def tabelaResultadosPT(listDirs, subDir = 'MyInstancesSameToolSets', totalPTL = 600):
    filesList = []

    fineshedJobsCountAcc = {}
    switchsAcc = {}
    switchsInstancesAcc = {} 
    unfineshedPriorityCountAcc = {}
    totalUnfinishedJobsCountAcc = {} 
    FinalSolutionAcc = {}  
    TimeAcc = {} 
    PTLAcc = {} 
    MCMCAcc = {} 
    BestInitialAcc = {} 
    MeanInitialAcc = {}

    # iterate over each directory and get the list of files inside it
    for dir in listDirs:
        files = os.listdir(f'{dir}/{subDir}')
        files = natsorted(files)
        for file in files:
            if file not in filesList: filesList.append(file)

            planejamento, machines, endInfo = rp.parseReport(f'{dir}/{subDir}/{file}')
            totalUnfinishedJobsCount = totalUnfinishedJobs(machines, planejamento)
            # print(endInfo)
            # exit(1)

            fineshedJobsCountAcc[file] = fineshedJobsCountAcc.get(file, []) + [endInfo['fineshedJobsCount']]
            totalUnfinishedJobsCountAcc[file] = totalUnfinishedJobsCountAcc.get(file, []) + [totalUnfinishedJobsCount]
            switchsAcc[file] = switchsAcc.get(file, []) + [endInfo['switchs']]
            switchsInstancesAcc[file] = switchsInstancesAcc.get(file, []) + [endInfo['switchsInstances']]
            unfineshedPriorityCountAcc[file] = unfineshedPriorityCountAcc.get(file, []) + [endInfo['unfineshedPriorityCount']]
            FinalSolutionAcc[file] = FinalSolutionAcc.get(file, []) + [endInfo['finalSolution']]
            TimeAcc[file] = TimeAcc.get(file, []) + [endInfo['Time']/1000]
            PTLAcc[file] = PTLAcc.get(file, []) + [endInfo['PTL']]
            MCMCAcc[file] = MCMCAcc.get(file, []) + [endInfo['MCMC']]
            BestInitialAcc[file] = BestInitialAcc.get(file, []) + [endInfo['bestInitial']]
            MeanInitialAcc[file] = MeanInitialAcc.get(file, []) + [endInfo['meanInitial']]

    modoPlanilha = False
    separador = '&'

    # TABELA 1
    for index, file in enumerate(filesList): 
        instancenameClear = file.split(",t=")[0]
        componentesDoNome = instancenameClear.split(',')
        totalTarefas = int(componentesDoNome[0].split('=')[1])
        taxaPrioridade = float(componentesDoNome[1].split('=')[1])
        taxaReentrancia = float(componentesDoNome[2].split('=')[1])

        endPrint = ''
        if index == len(filesList) - 1: endPrint = ' \\\\ \\hline'
        else : endPrint = ' \\\\'
        outputTeste = ((
            f'{totalTarefas} {separador} ' # n
            f'{taxaPrioridade} {separador} ' # p
            f'{taxaReentrancia} {separador} ' # r
            f'{statistics.mean(fineshedJobsCountAcc[file]):,.2f} {separador} '
            f'{statistics.mean(unfineshedPriorityCountAcc[file]):,.2f} {separador} '
            f'{statistics.mean(totalUnfinishedJobsCountAcc[file]):,.2f} {separador} '
            f'{statistics.mean(switchsInstancesAcc[file]):,.2f} {separador} '
            f'{statistics.mean(switchsAcc[file]):,.2f}'
            f'{endPrint}'
        ).replace('.', ',').replace('', ''))
        if(modoPlanilha): outputTeste = outputTeste.replace('&', ';').replace('\\\\', '').replace('\\hline', '')
        print(outputTeste)
        if((index+1) % 3 == 0 and not(index == len(files) - 1) and not modoPlanilha): print("\\hline")
    
    # TABELA 2
    print()
    for index, file in enumerate(filesList): 
        gap = (max(FinalSolutionAcc[file]) - statistics.mean(BestInitialAcc[file]))/max(FinalSolutionAcc[file]) * 100
        stdPercent = statistics.stdev(FinalSolutionAcc[file])/statistics.mean(FinalSolutionAcc[file]) * 100
        instancenameClear = file.split(",t=")[0]
        componentesDoNome = instancenameClear.split(',')
        totalTarefas = int(componentesDoNome[0].split('=')[1])
        taxaPrioridade = float(componentesDoNome[1].split('=')[1])
        taxaReentrancia = float(componentesDoNome[2].split('=')[1])

        endPrint = ''
        if index == len(filesList) - 1: endPrint = ' \\\\ \\hline'
        else : endPrint = ' \\\\'
        outputTeste = ((
            f'{totalTarefas} {separador} ' # n
            f'{taxaPrioridade} {separador} ' # p
            f'{taxaReentrancia} {separador} ' # r
            f'{statistics.mean(BestInitialAcc[file]):.2f} {separador} '
            # f'{max(BestInitialAcc[file]):.2f} {separador} '
            # f'{statistics.mean(MeanInitialAcc[file]):.2f} {separador} '
            f'{max(FinalSolutionAcc[file]):.2f} {separador} '
            f'{statistics.mean(FinalSolutionAcc[file]):.2f} {separador} '
            f'{(stdPercent):.2f} {separador} '
            f'{statistics.mean(TimeAcc[file]):.2f} {separador} '
            f'{(statistics.mean(PTLAcc[file])/totalPTL)*100:.2f} {separador} '
            f'{(gap):.2f}'
            f'{endPrint}'
        ).replace('.', ',').replace('', ''))
        if(modoPlanilha): outputTeste = outputTeste.replace('&', ';').replace('\\\\', '').replace('\\hline', '')
        print(outputTeste)
        if((index+1) % 3 == 0 and not(index == len(files) - 1) and not modoPlanilha): print("\\hline")


    # OUTROS
    # print(max(fineshedJobsCountAcc['n=600,p=0.25,r=0.5,t=3096,v6.csv']))
    # print(max(fineshedJobsCountAcc['n=600,p=0.50,r=0.5,t=3096,v7.csv']))
    # print(max(fineshedJobsCountAcc['n=600,p=0.75,r=0.5,t=3096,v8.csv']))

    # print(min(fineshedJobsCountAcc['n=600,p=0.25,r=0.5,t=3096,v6.csv']))
    # print(min(fineshedJobsCountAcc['n=600,p=0.50,r=0.5,t=3096,v7.csv']))
    # print(min(fineshedJobsCountAcc['n=600,p=0.75,r=0.5,t=3096,v8.csv']))

    # acc = []
    # for index, file in enumerate(filesList): 
    #     totalTarefas, taxaPrioridade, taxaReentrancia, numeroFerramentas = getFileParans(file)
    #     prioritariasNaoFinalizadas = statistics.mean(unfineshedPriorityCountAcc[file])
    #     porcentagemPrioritariasNaoFinalizadas = (prioritariasNaoFinalizadas/totalTarefas) * 100
    #     acc.append(porcentagemPrioritariasNaoFinalizadas)
    #     print(file, porcentagemPrioritariasNaoFinalizadas)
    
    # print(statistics.mean(acc))
    # print(max(acc))
    # print(min(acc))

    # for index, file in enumerate(filesList): 
        # print(min(TimeAcc[file]))


    # acc = []
    # for index, file in enumerate(filesList): 
    #     S0 = statistics.mean(BestInitialAcc[file])
    #     SStar = max(FinalSolutionAcc[file])
    #     S = statistics.mean(FinalSolutionAcc[file])
    #     gap = (SStar - S)/SStar * 100
    #     acc.append(gap)
    #     print(gap)     
    # print("mean ", statistics.mean(acc))

    # acc = []
    # for index, file in enumerate(filesList): 
    #     S0 = statistics.mean(BestInitialAcc[file])
    #     SStar = max(FinalSolutionAcc[file])
    #     S = statistics.mean(FinalSolutionAcc[file])
    #     gap = (SStar - S0)/SStar * 100
    #     acc.append(gap)
    #     print(gap)     
    # print("mean ", statistics.mean(acc))

def tabelaResultadosComparativa(listDirs, subDir = 'MyInstancesSameToolSets', totalPTL = 600):
    filesList = []

    fineshedJobsCountAcc = {}
    switchsAcc = {} 
    switchsInstancesAcc = {} 
    unfineshedPriorityCountAcc = {}
    totalUnfinishedJobsCountAcc = {} 
    FinalSolutionAcc = {}  
    TimeAcc = {} 
    PTLAcc = {} 
    MCMCAcc = {} 
    BestInitialAcc = {} 
    MeanInitialAcc = {}

    # iterate over each directory and get the list of files inside it
    for dir in listDirs:
        files = os.listdir(f'{dir}/{subDir}')
        files = natsorted(files)
        for file in files:
            if file not in filesList: filesList.append(file)

            planejamento, machines, endInfo = rp.parseReport(f'{dir}/{subDir}/{file}')
            totalUnfinishedJobsCount = totalUnfinishedJobs(machines, planejamento)
            # print(endInfo)
            # exit(1)

            fineshedJobsCountAcc[file] = fineshedJobsCountAcc.get(file, []) + [endInfo['fineshedJobsCount']]
            totalUnfinishedJobsCountAcc[file] = totalUnfinishedJobsCountAcc.get(file, []) + [totalUnfinishedJobsCount]
            switchsAcc[file] = switchsAcc.get(file, []) + [endInfo['switchs']]
            switchsInstancesAcc[file] = switchsInstancesAcc.get(file, []) + [endInfo['switchsInstances']]
            unfineshedPriorityCountAcc[file] = unfineshedPriorityCountAcc.get(file, []) + [endInfo['unfineshedPriorityCount']]
            FinalSolutionAcc[file] = FinalSolutionAcc.get(file, []) + [endInfo['finalSolution']]
            TimeAcc[file] = TimeAcc.get(file, []) + [endInfo['Time']/1000]
            PTLAcc[file] = PTLAcc.get(file, []) + [endInfo['PTL']]
            MCMCAcc[file] = MCMCAcc.get(file, []) + [endInfo['MCMC']]
            BestInitialAcc[file] = BestInitialAcc.get(file, []) + [endInfo['bestInitial']]
            MeanInitialAcc[file] = MeanInitialAcc.get(file, []) + [endInfo['meanInitial']]

    modoPlanilha = False
    separador = '&'

    folderNamePH = './output/Practitioner/MyInstancesSameToolSets/'
    filesPH = os.listdir(folderNamePH)
    filesPH = natsorted(filesPH) 
    fileWithPathPH = [f"{folderNamePH}/{file}" for file in filesPH if file.endswith(".csv")]

    folderNameModelo = './output/Modelo/MyInstancesSameToolSets/'
    filesModelo = os.listdir(folderNameModelo)
    filesModelo = natsorted(filesModelo) 
    fileWithPathModelo = [f"{folderNameModelo}/{file}" for file in filesModelo if file.endswith(".csv")]

    # print(len(fileWithPathPH))
    # print(len(filesList))
    # exit(1)
    for index, file in enumerate(filesList): 
        sStar = max(FinalSolutionAcc[file])
        s = statistics.mean(FinalSolutionAcc[file])
        gapPT = (sStar - s)/sStar * 100

        instancenameClear = file.split(",t=")[0]
        componentesDoNome = instancenameClear.split(',')
        totalTarefas = int(componentesDoNome[0].split('=')[1])
        taxaPrioridade = float(componentesDoNome[1].split('=')[1])
        taxaReentrancia = float(componentesDoNome[2].split('=')[1])
        
        planejamentoPH, machinesPH, endInfoPH = rp.parseReport(fileWithPathPH[index])
        resultadoPH = endInfoPH["finalSolution"]
        gapPTPH = (s - resultadoPH)/s * 100

        gapPTModelo = "-"
        resultadoModelo = "-"
        if(index <= len(fileWithPathModelo) - 1):
            planejamentoModelo, machinesModelo, endInfoModelo = rp.parseReport(fileWithPathModelo[index])
            resultadoModelo = endInfoModelo["finalSolution"]
            gapPTModelo = (s - resultadoModelo)/s * 100
            gapPTModelo = f'{gapPTModelo:.2f}'
            resultadoModelo = f'{resultadoModelo:.2f}'

        endPrint = ''
        if index == len(filesList) - 1: endPrint = ' \\\\ \\hline'
        else : endPrint = ' \\\\'
        outputTeste = ((
            f'{totalTarefas} {separador} ' # n
            f'{taxaPrioridade} {separador} ' # p
            f'{taxaReentrancia} {separador} ' # r          
            f'{sStar:.2f} {separador} ' # S* PT
            f'{s:.2f} {separador} ' # S PT  
            f'{gapPT:.2f} {separador}' # gap entre S* e S
            f'{resultadoPH:.2f} {separador} ' # S PH
            f'{gapPTPH:.2f} {separador}' # gap entre SPT e SPH
            f'{resultadoModelo} {separador} ' # S Modelo
            f'{gapPTModelo}' # gap entre SPT e SModelo
            f'{endPrint}'
        ).replace('.', ',').replace(',00', ''))
        if(modoPlanilha): outputTeste = outputTeste.replace('&', ';').replace('\\\\', '').replace('\\hline', '')
        print(outputTeste)
        if((index+1) % 3 == 0 and not(index == len(files) - 1) and not modoPlanilha): print("\\hline")



# ---------------------------------------------------------------------------------------------------
# MAIN
# ---------------------------------------------------------------------------------------------------

def main():
    folderName = sys.argv[1]
    option = sys.argv[2]

    fileWithPath = []
    if(option == '0'):
        fileWithPath = [folderName]
        option = '1'
    elif(option == '4' or option == '5'):
        files = os.listdir(folderName)
        files = natsorted(files) 
        fileWithPath = [f"{folderName}/{file}" for file in files]
    else: 
        files = os.listdir(folderName)
        files = natsorted(files) 
        fileWithPath = [f"{folderName}/{file}" for file in files if file.endswith(".csv")]

    if option == '1': validarPasta(fileWithPath)  
    if option == '2': tabelaResultadosPractitioner(fileWithPath)
    if option == '3': tabelaResultadosModelo(fileWithPath)
    if option == '4': tabelaResultadosPT(fileWithPath)
    if option == '5': tabelaResultadosComparativa(fileWithPath)

def mainCollection():
    pass

if __name__ == "__main__":
    main()
