# passar pelo validador
# quantas vezes as precedencia foi quebrada
# analisar os tempos e os resultados = formato facil de passar pra planilha

import sys
import os
from natsort import natsorted

import uteis.reportParser as rp
import uteis.validador as vd
import uteis.loadData as ld

def validarPasta(files):

    for report in files:
        print(f"---Validating {report}---")
    
        planejamento, machines, endInfo = rp.parseReport(report)
        toolSets = ld.loadToolSet(planejamento['toolSetFileName'])
        jobs = ld.loadJobs(planejamento['jobsFileName'])

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

def totalUnfinishedJobs(machines, planejamento):
    jobs = ld.loadJobs(planejamento['jobsFileName'])
    total = len(jobs)
    for machine in machines:
        total -= len(machine)
    return total

def analisarValores(files):
    precedenciasViloladas =  verificarPrecedencia(files)
    for index, report in enumerate(files):
        planejamento, machines, endInfo = rp.parseReport(report)
        totalUnfinishedJobsCount = totalUnfinishedJobs(machines, planejamento)
        print(f'{index + 1} {endInfo["fineshedPriorityCount"]} {endInfo["unfinesedPriorityCount"]} {totalUnfinishedJobsCount} {endInfo["switchsInstances"]} {endInfo["switchs"]} {endInfo["cost"]} {endInfo["timeSpent"]/1000:.2f}'.replace('.', ',') + f' {precedenciasViloladas[index]}')

# ---------------------------------------------------------------------------------------------------
# MAIN
# ---------------------------------------------------------------------------------------------------

def main():
    folderName = sys.argv[1]
    files = os.listdir(folderName)
    files = natsorted(files) 
    fileWithPath = [f"{folderName}/{file}" for file in files]

    # get the option from the user 
    option = sys.argv[2]

    if option == '1': validarPasta(fileWithPath)  
    if option == '2': analisarValores(fileWithPath)
    if option == '3': verificarPrecedencia(fileWithPath)
    if option == '4': verificarPrecedenciaAsSingleMachine(fileWithPath)
    if option == '5': verificarPares(fileWithPath)

def mainCollection():
    pass

if __name__ == "__main__":
    main()
