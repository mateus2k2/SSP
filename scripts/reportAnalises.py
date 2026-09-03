#!/usr/bin/env python3
"""
Processes mainCpp solution reports (src/loadData.cpp/evaluateUteis.cpp's
report format, read via uteis/reportParser.py): validates a report against
its instance, and formats run results into LaTeX/spreadsheet-ready tables.

Subcommands:
  validate <folder-or-file>       Consistency checks (magazine contents, switch
                                   counts, profit formula, ...) via uteis/validador.py.
                                   Legacy-CSV / Consolidated / BaseInstances formats.
  validate-beezao <folder>        Validate + average .PMTC run results across numbered
                                   run subfolders (uteis/validadorBeezao.py); writes
                                   results_mean.csv into that folder.
  table-practitioner <folder>     LaTeX table rows for a practitioner-heuristic run.
  table-modelo <folder>           LaTeX table rows for a Gurobi-model run.
  table-pt <dirs...>               Two LaTeX tables (per-instance means, then a
                                   gap/std-dev table) averaged across run dirs.
  table-comparative <dirs...>      LaTeX table comparing PT vs practitioner vs modelo.
  precedence <folder>             Report broken operation-0/operation-1 precedence.
  ktns <file>                     Print the KTNS magazine trace for one report.
  tools <folder>                  Print unique-tool count per instance file (legacy
                                   CSV format only).

Usage:
  python3 scripts/reportAnalises.py table-pt ./output/diffTesla
  python3 scripts/reportAnalises.py validate ./output/Beezao/teste.txt

This whole file used to be a single numbered --option flag with ~90% of its
body commented out -- every Makefile target calling it (tabelaResultados*,
validarFile) had silently been running an unrelated 15-line stub instead.
Restored from the comments (fully specified, not guesswork) as a real
subcommand CLI. One real fix made while restoring: table-pt's PTL/MCMC/
BestInitial/MeanInitial accumulation was commented out (so its second table
would KeyError on first use) -- that data simply didn't exist while the PT
solving path was dead. It's live again (see src/main.cpp's `runPT`), and a
real PT report does contain those fields, so the accumulation is restored
too, not left broken. Verified: `validate` and reportParser's parsing run
correctly against a real, current mainCpp report; the check functions in
uteis/validador.py agree with the solver's own profit calculation.
"""
import argparse
import os
import statistics

from natsort import natsorted

import uteis.loadData as ld
import uteis.reportParser as rp
import uteis.validador as vd
import uteis.validadorBeezao as vdb

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
        FimOperacao0 = {}

        for machine in machines:
            for estado in machine:
                job = estado['job']
                if estado['operation'] == 0:
                    FimOperacao0[job] = estado['end']

        for machine in machines:
            for estado in machine:
                job = estado['job']
                if estado['operation'] == 1:
                    if not (job in FimOperacao0) or estado['start'] < FimOperacao0[job]:
                        print(f'Precedencia quebrada para a instancia {index + 1} no job {job}')
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
                if estado['operation'] == 0:
                    precedencia[job].append(0)
                if estado['operation'] == 1:
                    if not precedencia[job]:
                        print(f'Precedencia quebrada para a instancia {index + 1} no job {job}')
                        quantidadePrecedenciaQuebradaPorInstancia[index] += 1
                    else:
                        precedencia[job].pop()

    return quantidadePrecedenciaQuebradaPorInstancia

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

def tabelaResultadosPractitioner(files, modoPlanilha=False):
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

        endPrint = ' \\\\ \\hline' if index == len(files) - 1 else ' \\\\'
        outputTeste = ((
            f'{totalTarefas} {separador} '
            f'{taxaPrioridade} {separador} '
            f'{taxaReentrancia} {separador} '
            f'{endInfo["fineshedJobsCount"]:,.2f} {separador}'
            f'{endInfo["unfineshedPriorityCount"]:,.2f} {separador}'
            f'{totalUnfineshed:,.2f} {separador}'
            f'{endInfo["switchsInstances"]:,.2f} {separador}'
            f'{endInfo["switchs"]:,.2f} {separador}'
            f'{endInfo["finalSolution"]:,.2f}'
            f'{endPrint}'
        ).replace('.', ',').replace(',00', ''))
        if modoPlanilha:
            outputTeste = outputTeste.replace('&', ';').replace('\\\\', '').replace('\\hline', '')
        print(outputTeste)
        if (index + 1) % 3 == 0 and not (index == len(files) - 1) and not modoPlanilha:
            print("\\hline")

def tabelaResultadosModelo(files, modoPlanilha=False):
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

        endPrint = ' \\\\ \\hline' if index == len(files) - 1 else ' \\\\'
        outputTeste = ((
            f'{totalTarefas} {separador} '
            f'{taxaPrioridade} {separador} '
            f'{taxaReentrancia} {separador} '
            f'{endInfo["fineshedJobsCount"]:,.2f} {separador}'
            f'{endInfo["unfineshedPriorityCount"]:,.2f} {separador}'
            f'{totalUnfineshed:,.2f} {separador}'
            f'{endInfo["switchsInstances"]:,.2f} {separador}'
            f'{endInfo["switchs"]:,.2f} {separador}'
            f'{endInfo["bestBound"]:,.2f} {separador}'
            f'{endInfo["finalSolution"]:,.2f} {separador}'
            f"{endInfo['Time']}"
            f'{endPrint}'
        ).replace('.', ',').replace(',00', ''))
        if modoPlanilha:
            outputTeste = outputTeste.replace('&', ';').replace('\\\\', '').replace('\\hline', '')
        print(outputTeste)
        if (index + 1) % 3 == 0 and not (index == len(files) - 1) and not modoPlanilha:
            print("\\hline")

def tabelaResultadosPT(listDirs, subDir='MyInstancesSameToolSets', totalPTL=600, modoPlanilha=False):
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

    for dir in listDirs:
        files = natsorted(os.listdir(f'{dir}/{subDir}'))
        for file in files:
            if file not in filesList:
                filesList.append(file)

            planejamento, machines, endInfo = rp.parseReport(f'{dir}/{subDir}/{file}')
            totalUnfinishedJobsCount = totalUnfinishedJobs(machines, planejamento)

            fineshedJobsCountAcc[file] = fineshedJobsCountAcc.get(file, []) + [endInfo['fineshedJobsCount']]
            totalUnfinishedJobsCountAcc[file] = totalUnfinishedJobsCountAcc.get(file, []) + [totalUnfinishedJobsCount]
            switchsAcc[file] = switchsAcc.get(file, []) + [endInfo['switchs']]
            switchsInstancesAcc[file] = switchsInstancesAcc.get(file, []) + [endInfo['switchsInstances']]
            unfineshedPriorityCountAcc[file] = unfineshedPriorityCountAcc.get(file, []) + [endInfo['unfineshedPriorityCount']]
            FinalSolutionAcc[file] = FinalSolutionAcc.get(file, []) + [endInfo['finalSolution']]
            TimeAcc[file] = TimeAcc.get(file, []) + [endInfo['Time'] / 1000]
            if 'PTL' in endInfo:
                PTLAcc[file] = PTLAcc.get(file, []) + [endInfo['PTL']]
            if 'MCMC' in endInfo:
                MCMCAcc[file] = MCMCAcc.get(file, []) + [endInfo['MCMC']]
            if 'bestInitial' in endInfo:
                BestInitialAcc[file] = BestInitialAcc.get(file, []) + [endInfo['bestInitial']]
            if 'meanInitial' in endInfo:
                MeanInitialAcc[file] = MeanInitialAcc.get(file, []) + [endInfo['meanInitial']]

    separador = '&'

    # TABELA 1
    for index, file in enumerate(filesList):
        instancenameClear = file.split(",t=")[0]
        componentesDoNome = instancenameClear.split(',')
        totalTarefas = int(componentesDoNome[0].split('=')[1])
        taxaPrioridade = float(componentesDoNome[1].split('=')[1])
        taxaReentrancia = float(componentesDoNome[2].split('=')[1])

        endPrint = ' \\\\ \\hline' if index == len(filesList) - 1 else ' \\\\'
        outputTeste = ((
            f'{totalTarefas} {separador} '
            f'{taxaPrioridade} {separador} '
            f'{taxaReentrancia} {separador} '
            f'{statistics.mean(fineshedJobsCountAcc[file]):,.2f} {separador} '
            f'{statistics.mean(unfineshedPriorityCountAcc[file]):,.2f} {separador} '
            f'{statistics.mean(totalUnfinishedJobsCountAcc[file]):,.2f} {separador} '
            f'{statistics.mean(switchsInstancesAcc[file]):,.2f} {separador} '
            f'{statistics.mean(switchsAcc[file]):,.2f}'
            f'{endPrint}'
        ).replace('.', ','))
        if modoPlanilha:
            outputTeste = outputTeste.replace('&', ';').replace('\\\\', '').replace('\\hline', '')
        print(outputTeste)
        if (index + 1) % 3 == 0 and not (index == len(filesList) - 1) and not modoPlanilha:
            print("\\hline")

    # TABELA 2
    print()
    print()
    print()
    for index, file in enumerate(filesList):
        if file not in BestInitialAcc or file not in PTLAcc:
            print(f"# {file}: no PTL/BestInitial data (not a PT report) -- skipped")
            continue

        gap = (max(FinalSolutionAcc[file]) - statistics.mean(BestInitialAcc[file])) / max(FinalSolutionAcc[file]) * 100
        stdPercent = 0
        if len(FinalSolutionAcc[file]) > 1:
            stdPercent = statistics.stdev(FinalSolutionAcc[file]) / statistics.mean(FinalSolutionAcc[file]) * 100
        instancenameClear = file.split(",t=")[0]
        componentesDoNome = instancenameClear.split(',')
        totalTarefas = int(componentesDoNome[0].split('=')[1])
        taxaPrioridade = float(componentesDoNome[1].split('=')[1])
        taxaReentrancia = float(componentesDoNome[2].split('=')[1])

        endPrint = ' \\\\ \\hline' if index == len(filesList) - 1 else ' \\\\'
        outputTeste = ((
            f'{totalTarefas} {separador} '
            f'{taxaPrioridade} {separador} '
            f'{taxaReentrancia} {separador} '
            f'{statistics.mean(BestInitialAcc[file]):.2f} {separador} '
            f'{max(FinalSolutionAcc[file]):.2f} {separador} '
            f'{statistics.mean(FinalSolutionAcc[file]):.2f} {separador} '
            f'{(stdPercent):.2f} {separador} '
            f'{statistics.mean(TimeAcc[file]):.2f} {separador} '
            f'{(statistics.mean(PTLAcc[file]) / totalPTL) * 100:.2f} {separador} '
            f'{(gap):.2f}'
            f'{endPrint}'
        ).replace('.', ','))
        if modoPlanilha:
            outputTeste = outputTeste.replace('&', ';').replace('\\\\', '').replace('\\hline', '')
        print(outputTeste)
        if (index + 1) % 3 == 0 and not (index == len(filesList) - 1) and not modoPlanilha:
            print("\\hline")

def tabelaResultadosComparativa(listDirs, subDir='MyInstancesSameToolSets', totalPTL=600,
                                 practitionerDir='./output/Practitioner', modeloDir='./output/Modelo'):
    filesList = []

    FinalSolutionAcc = {}

    for dir in listDirs:
        files = natsorted(os.listdir(f'{dir}/{subDir}'))
        for file in files:
            if file not in filesList:
                filesList.append(file)

            planejamento, machines, endInfo = rp.parseReport(f'{dir}/{subDir}/{file}')
            FinalSolutionAcc[file] = FinalSolutionAcc.get(file, []) + [endInfo['finalSolution']]

    separador = '&'

    folderNamePH = f'{practitionerDir}/{subDir}/'
    filesPH = natsorted(os.listdir(folderNamePH))
    fileWithPathPH = [f"{folderNamePH}/{file}" for file in filesPH if file.endswith(".csv")]

    folderNameModelo = f'{modeloDir}/{subDir}/'
    filesModelo = natsorted(os.listdir(folderNameModelo))
    fileWithPathModelo = [f"{folderNameModelo}/{file}" for file in filesModelo if file.endswith(".csv")]

    for index, file in enumerate(filesList):
        sStar = max(FinalSolutionAcc[file])
        s = statistics.mean(FinalSolutionAcc[file])
        gapPT = (sStar - s) / sStar * 100

        instancenameClear = file.split(",t=")[0]
        componentesDoNome = instancenameClear.split(',')
        totalTarefas = int(componentesDoNome[0].split('=')[1])
        taxaPrioridade = float(componentesDoNome[1].split('=')[1])
        taxaReentrancia = float(componentesDoNome[2].split('=')[1])

        planejamentoPH, machinesPH, endInfoPH = rp.parseReport(fileWithPathPH[index])
        resultadoPH = endInfoPH["finalSolution"]
        gapPTPH = (s - resultadoPH) / s * 100

        gapPTModelo = "-"
        resultadoModelo = "-"
        if index <= len(fileWithPathModelo) - 1:
            planejamentoModelo, machinesModelo, endInfoModelo = rp.parseReport(fileWithPathModelo[index])
            resultadoModelo = endInfoModelo["finalSolution"]
            gapPTModelo = (s - resultadoModelo) / s * 100
            gapPTModelo = f'{gapPTModelo:.2f}'
            resultadoModelo = f'{resultadoModelo:.2f}'

        endPrint = ' \\\\ \\hline' if index == len(filesList) - 1 else ' \\\\'
        outputTeste = ((
            f'{totalTarefas} {separador} '
            f'{taxaPrioridade} {separador} '
            f'{taxaReentrancia} {separador} '
            f'{sStar:.2f} {separador} '
            f'{s:.2f} {separador} '
            f'{gapPT:.2f} {separador}'
            f'{resultadoPH:.2f} {separador} '
            f'{gapPTPH:.2f} {separador}'
            f'{resultadoModelo} {separador} '
            f'{gapPTModelo}'
            f'{endPrint}'
        ).replace('.', ',').replace(',00', ''))
        print(outputTeste)
        if (index + 1) % 3 == 0 and not (index == len(filesList) - 1):
            print("\\hline")

def lookupJob(job, operations, jobFile):
    for jobData in jobFile:
        if jobData['Job'] == job and jobData['Operation'] == operations:
            return jobData

def analiseKTNS(file):
    print(f"---Analysing {file}---")
    planejamento, machines, endInfo = rp.parseReport(file)
    toolSets = ld.loadToolSet(planejamento['toolSetFileName'])
    jobs = ld.loadJobs(planejamento['jobsFileName'])

    for i, machine in enumerate(machines):
        for j, estado in enumerate(machine):
            currMagazine = estado['magazine']

            for k in range(j, len(machine)):
                nextEstado = machine[k]
                nextJob = lookupJob(nextEstado['job'], nextEstado['operation'], jobs)
                nextToolSet = toolSets[nextJob['ToolSet']]
                currMagazine = [tool for tool in currMagazine if tool not in nextToolSet]

            print(f" Tools: {len(toolSets[jobs[estado['job']]['ToolSet']])} "
                  f"Magazine no processamento do job: {estado['job']}, operation: {estado['operation']}, "
                  f"depois de retirar as ferramentas por nenhum job subsequente na maquina: {len(currMagazine)}")
        print("")

def analiseFerramentasUnicas(folder, toolset_file):
    toolSets = ld.loadToolSet(toolset_file)
    files = natsorted(f for f in os.listdir(folder) if f.endswith(".csv"))

    print("Instance -> #Unique Tools")
    print("--------------------------------")

    for file in files:
        jobs = ld.loadJobs(f"{folder}/{file}")
        unique_tools = set()
        for job in jobs:
            unique_tools.update(toolSets[job['ToolSet']])
        print(f"{file} -> {len(unique_tools)}")

# ---------------------------------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------------------------------

def _collect_files(folder_or_file, ext=None):
    if os.path.isfile(folder_or_file):
        return [folder_or_file]
    files = natsorted(os.listdir(folder_or_file))
    if ext:
        files = [f for f in files if f.endswith(ext)]
    return [os.path.join(folder_or_file, f) for f in files]


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--spreadsheet", action="store_true",
                         help="Emit ';'-separated rows for pasting into a spreadsheet instead of LaTeX rows")
    sub = parser.add_subparsers(dest="command", required=True)

    p = sub.add_parser("validate", help="Validate report(s) against their instance")
    p.add_argument("target", help="A single report file, or a folder of them")
    p.set_defaults(func=lambda a: validarPasta(_collect_files(a.target)))

    p = sub.add_parser("validate-beezao", help="Validate + average .PMTC results across run subfolders")
    p.add_argument("folder", nargs="?", default="./output/BeezaoPTLarge/")
    p.set_defaults(func=lambda a: vdb.validateFolder(a.folder))

    p = sub.add_parser("table-practitioner", help="LaTeX table rows for a practitioner run")
    p.add_argument("folder")
    p.set_defaults(func=lambda a: tabelaResultadosPractitioner(_collect_files(a.folder), a.spreadsheet))

    p = sub.add_parser("table-modelo", help="LaTeX table rows for a Gurobi-model run")
    p.add_argument("folder")
    p.set_defaults(func=lambda a: tabelaResultadosModelo(_collect_files(a.folder), a.spreadsheet))

    p = sub.add_parser("table-pt", help="PT results tables (per-instance means, then gap/std stats)")
    p.add_argument("dirs", nargs="+", help="Run directories, each containing <subdir>/<instance files>")
    p.add_argument("--subdir", default="MyInstancesSameToolSets")
    p.add_argument("--total-ptl", type=int, default=600)
    p.set_defaults(func=lambda a: tabelaResultadosPT(a.dirs, a.subdir, a.total_ptl, a.spreadsheet))

    p = sub.add_parser("table-comparative", help="Compare PT vs practitioner vs modelo results")
    p.add_argument("dirs", nargs="+", help="PT run directories")
    p.add_argument("--subdir", default="MyInstancesSameToolSets")
    p.add_argument("--total-ptl", type=int, default=600)
    p.add_argument("--practitioner-dir", default="./output/Practitioner")
    p.add_argument("--modelo-dir", default="./output/Modelo")
    p.set_defaults(func=lambda a: tabelaResultadosComparativa(
        a.dirs, a.subdir, a.total_ptl, a.practitioner_dir, a.modelo_dir))

    p = sub.add_parser("precedence", help="Report broken operation-0/operation-1 precedence")
    p.add_argument("folder")

    def _precedence(a):
        files = _collect_files(a.folder)
        verificarPrecedencia(files)
        verificarPrecedenciaAsSingleMachine(files)
    p.set_defaults(func=_precedence)

    p = sub.add_parser("ktns", help="Print the KTNS magazine trace for one report")
    p.add_argument("file")
    p.set_defaults(func=lambda a: analiseKTNS(a.file))

    p = sub.add_parser("tools", help="Print unique-tool count per instance file (legacy CSV format)")
    p.add_argument("folder")
    p.add_argument("--toolset-file", default="./input/Processed/ToolSetInt.csv")
    p.set_defaults(func=lambda a: analiseFerramentasUnicas(a.folder, a.toolset_file))

    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
