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
        vd.checkOrder(machines)
        print()
        vd.checkProfit(machines, endInfo, jobs, planejamento)
        print()
        # vd.newKTNS(machines, toolSets, jobs, planejamento)
        # print()

def verificarPrecedencia(files):
    for report in files:
        pass

def analisarTempoNãoSupervisionadoUsado(files):
    for report in files:
        pass

def analisarValores(files):
    for report in files:
        pass

# ---------------------------------------------------------------------------------------------------
# MAIN
# ---------------------------------------------------------------------------------------------------

def main():
    folderName = sys.argv[1]
    files = os.listdir(folderName)
    files = natsorted(files) 
    fileWithPath = [f"{folderName}/{file}" for file in files]

    validarPasta(fileWithPath)
    verificarPrecedencia(fileWithPath)
    analisarValores(fileWithPath)
    analisarTempoNãoSupervisionadoUsado(fileWithPath)

if __name__ == "__main__":
    main()