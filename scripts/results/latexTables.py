#!/usr/bin/env python3
"""
Formats mainCpp solution reports (read via ssp/reports.py) into LaTeX (or
';'-separated, with --spreadsheet) table rows, one row per instance.

The rows print the counters exactly as the reports state them. To check the
reports use results/validateRuns.py; for the full results spreadsheet (with
validated values) use results/buildSpreadsheet.py.

Subcommands:
  table-practitioner <folder>     rows for a practitioner-heuristic run.
  table-modelo <folder>           rows for a Gurobi-model run.
  table-ga <dirs...>              detailed GA table averaged across run dirs.
  table-pt <dirs...>              two tables (per-instance means, then a
                                  gap/std-dev table) averaged across run dirs.
  table-comparative <dirs...>     PT vs practitioner vs modelo.

Usage:
  python3 scripts/results/latexTables.py table-pt ./output/diffTesla/*
  python3 scripts/results/latexTables.py table-practitioner output-final/same-toolset/Practitioner-single-run
"""
import argparse
import os
import re
import statistics
import sys

from natsort import natsorted

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))  # scripts/, for ssp
from ssp import instances  # noqa: E402
from ssp import reports as rp  # noqa: E402
from ssp import results  # noqa: E402
from ssp import validation as validador  # noqa: E402

# ---------------------------------------------------------------------------------------------------
# ANALISES
# ---------------------------------------------------------------------------------------------------

def totalUnfinishedJobs(machines, planejamento):
    total = instances.load_instance(planejamento['jobsFileName']).n_ops
    for machine in machines:
        total -= len(machine)
    return total

def nameParams(path):
    """(n, p, r) from a report file name: 'n=600,p=0.25,r=0.5,t=3096,v6.csv' or,
    for the base instances, '2M1_n=50,r=0.5,t=86,v0.txt' (p = 0.5 for every class)."""
    name = os.path.basename(path)
    m = results.GRID_NAME.search(name)
    if m:
        return int(m.group(1)), float(m.group(2)), float(m.group(3))
    _, n, r = results.BASE_NAME.search(name).groups()
    return int(n), results.BASE_P, float(r)

def semDecimaisZerados(linha):
    """'150,00' -> '150' for whole numbers, leaving '0,00542164' alone."""
    return re.sub(r',00(?!\d)', '', linha)

def reportsIn(folder):
    """Report files in a folder, natural order (skips Gurobi .sol/.lp logs and notes)."""
    return [f for f in natsorted(os.listdir(folder))
            if not f.endswith(('.sol', '.lp', '.md')) and validador.is_report(os.path.join(folder, f))]

def tabelaResultadosPractitioner(files, modoPlanilha=False):
    separador = '&'
    for index, report in enumerate(files):
        planejamento, machines, endInfo = rp.parseReport(report)
        totalUnfineshed = totalUnfinishedJobs(machines, planejamento)

        totalTarefas, taxaPrioridade, taxaReentrancia = nameParams(report)

        endPrint = ' \\\\ \\hline' if index == len(files) - 1 else ' \\\\'
        outputTeste = ((
            f'{totalTarefas} {separador} '
            f'{taxaPrioridade} {separador} '
            f'{taxaReentrancia} {separador} '
            f'{endInfo["fineshedJobsCount"]:.2f} {separador}'
            f'{endInfo["unfineshedPriorityCount"]:.2f} {separador}'
            f'{totalUnfineshed:.2f} {separador}'
            f'{endInfo["switchsInstances"]:.2f} {separador}'
            f'{endInfo["switchs"]:.2f} {separador}'
            f'{endInfo["finalSolution"]:.2f}'
            f'{endPrint}'
        ).replace('.', ','))
        outputTeste = semDecimaisZerados(outputTeste)
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

        totalTarefas, taxaPrioridade, taxaReentrancia = nameParams(report)

        endPrint = ' \\\\ \\hline' if index == len(files) - 1 else ' \\\\'
        outputTeste = ((
            f'{totalTarefas} {separador} '
            f'{taxaPrioridade} {separador} '
            f'{taxaReentrancia} {separador} '
            f'{endInfo["fineshedJobsCount"]:.2f} {separador}'
            f'{endInfo["unfineshedPriorityCount"]:.2f} {separador}'
            f'{totalUnfineshed:.2f} {separador}'
            f'{endInfo["switchsInstances"]:.2f} {separador}'
            f'{endInfo["switchs"]:.2f} {separador}'
            f'{endInfo["bestBound"]:.2f} {separador}'
            f'{endInfo["finalSolution"]:.2f} {separador}'
            f"{endInfo['Time']}"
            f'{endPrint}'
        ).replace('.', ','))
        outputTeste = semDecimaisZerados(outputTeste)
        if modoPlanilha:
            outputTeste = outputTeste.replace('&', ';').replace('\\\\', '').replace('\\hline', '')
        print(outputTeste)
        if (index + 1) % 3 == 0 and not (index == len(files) - 1) and not modoPlanilha:
            print("\\hline")

def tabelaDetalhadaGA(listDirs, subDir='MyInstancesSameToolSets', modoPlanilha=False):
    """Per-instance breakdown table, everything averaged across listDirs (e.g.
    output/GATestes/1..10): finished/unfinished-priority/total-unfinished
    tasks, switch instances, tool switches, final solution, and time. There's
    no "Best Bound" for a GA run (that's a Gurobi/exact-method concept, not
    something a GA report carries) -- that column is always "-"."""
    filesList = []
    fineshedJobsCountAcc = {}
    unfineshedPriorityCountAcc = {}
    totalUnfinishedJobsCountAcc = {}
    switchsInstancesAcc = {}
    switchsAcc = {}
    FinalSolutionAcc = {}
    TimeAcc = {}

    for dir in listDirs:
        files = reportsIn(f'{dir}/{subDir}')
        for file in files:
            if file not in filesList:
                filesList.append(file)

            planejamento, machines, endInfo = rp.parseReport(f'{dir}/{subDir}/{file}')
            totalUnfinishedJobsCount = totalUnfinishedJobs(machines, planejamento)

            fineshedJobsCountAcc[file] = fineshedJobsCountAcc.get(file, []) + [endInfo['fineshedJobsCount']]
            unfineshedPriorityCountAcc[file] = unfineshedPriorityCountAcc.get(file, []) + [endInfo['unfineshedPriorityCount']]
            totalUnfinishedJobsCountAcc[file] = totalUnfinishedJobsCountAcc.get(file, []) + [totalUnfinishedJobsCount]
            switchsInstancesAcc[file] = switchsInstancesAcc.get(file, []) + [endInfo['switchsInstances']]
            switchsAcc[file] = switchsAcc.get(file, []) + [endInfo['switchs']]
            FinalSolutionAcc[file] = FinalSolutionAcc.get(file, []) + [endInfo['finalSolution']]
            TimeAcc[file] = TimeAcc.get(file, []) + [endInfo['Time'] / 1000]

    separador = '&'
    for index, file in enumerate(filesList):
        totalTarefas, taxaPrioridade, taxaReentrancia = nameParams(file)

        endPrint = ' \\\\ \\hline' if index == len(filesList) - 1 else ' \\\\'
        outputTeste = ((
            f'{totalTarefas} {separador} '
            f'{taxaPrioridade} {separador} '
            f'{taxaReentrancia} {separador} '
            f'{statistics.mean(fineshedJobsCountAcc[file]):.2f} {separador} '
            f'{statistics.mean(unfineshedPriorityCountAcc[file]):.2f} {separador} '
            f'{statistics.mean(totalUnfinishedJobsCountAcc[file]):.2f} {separador} '
            f'{statistics.mean(switchsInstancesAcc[file]):.2f} {separador} '
            f'{statistics.mean(switchsAcc[file]):.2f} {separador} '
            f'- {separador} '
            f'{statistics.mean(FinalSolutionAcc[file]):.2f} {separador} '
            f'{statistics.mean(TimeAcc[file]):.2f}'
            f'{endPrint}'
        ).replace('.', ','))
        outputTeste = semDecimaisZerados(outputTeste)
        if modoPlanilha:
            outputTeste = outputTeste.replace('&', ';').replace('\\\\', '').replace('\\hline', '')
        print(outputTeste)
        if (index + 1) % 3 == 0 and not (index == len(filesList) - 1) and not modoPlanilha:
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
        files = reportsIn(f'{dir}/{subDir}')
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
        totalTarefas, taxaPrioridade, taxaReentrancia = nameParams(file)

        endPrint = ' \\\\ \\hline' if index == len(filesList) - 1 else ' \\\\'
        outputTeste = ((
            f'{totalTarefas} {separador} '
            f'{taxaPrioridade} {separador} '
            f'{taxaReentrancia} {separador} '
            f'{statistics.mean(fineshedJobsCountAcc[file]):.2f} {separador} '
            f'{statistics.mean(unfineshedPriorityCountAcc[file]):.2f} {separador} '
            f'{statistics.mean(totalUnfinishedJobsCountAcc[file]):.2f} {separador} '
            f'{statistics.mean(switchsInstancesAcc[file]):.2f} {separador} '
            f'{statistics.mean(switchsAcc[file]):.2f}'
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
        totalTarefas, taxaPrioridade, taxaReentrancia = nameParams(file)

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
        files = reportsIn(f'{dir}/{subDir}')
        for file in files:
            if file not in filesList:
                filesList.append(file)

            planejamento, machines, endInfo = rp.parseReport(f'{dir}/{subDir}/{file}')
            FinalSolutionAcc[file] = FinalSolutionAcc.get(file, []) + [endInfo['finalSolution']]

    separador = '&'

    folderNamePH = f'{practitionerDir}/{subDir}/'
    filesPH = reportsIn(folderNamePH)
    fileWithPathPH = [f"{folderNamePH}/{file}" for file in filesPH if file.endswith(".csv")]

    folderNameModelo = f'{modeloDir}/{subDir}/'
    filesModelo = reportsIn(folderNameModelo)
    fileWithPathModelo = [f"{folderNameModelo}/{file}" for file in filesModelo if file.endswith(".csv")]

    for index, file in enumerate(filesList):
        sStar = max(FinalSolutionAcc[file])
        s = statistics.mean(FinalSolutionAcc[file])
        gapPT = (sStar - s) / sStar * 100

        totalTarefas, taxaPrioridade, taxaReentrancia = nameParams(file)

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
        ).replace('.', ','))
        outputTeste = semDecimaisZerados(outputTeste)
        print(outputTeste)
        if (index + 1) % 3 == 0 and not (index == len(filesList) - 1):
            print("\\hline")

# ---------------------------------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------------------------------

def _collect_files(folder_or_file, ext=None):
    if os.path.isfile(folder_or_file):
        return [folder_or_file]
    files = reportsIn(folder_or_file)
    if ext:
        files = [f for f in files if f.endswith(ext)]
    return [os.path.join(folder_or_file, f) for f in files]


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest="command", required=True)

    # Shared by every table-* subcommand. A parent parser (rather than a flag on the top-level
    # `parser`) so --spreadsheet can be given after the subcommand name, e.g.
    # `table-ga output/GATestes/1 --spreadsheet` -- argparse subparsers don't accept a parent
    # parser's flags positioned after the subcommand, only its own.
    spreadsheet_parser = argparse.ArgumentParser(add_help=False)
    spreadsheet_parser.add_argument("--spreadsheet", action="store_true",
                                     help="Emit ';'-separated rows for pasting into a spreadsheet instead of LaTeX rows")

    p = sub.add_parser("table-practitioner", parents=[spreadsheet_parser], help="LaTeX table rows for a practitioner run")
    p.add_argument("folder")
    p.set_defaults(func=lambda a: tabelaResultadosPractitioner(_collect_files(a.folder), a.spreadsheet))

    p = sub.add_parser("table-modelo", parents=[spreadsheet_parser], help="LaTeX table rows for a Gurobi-model run")
    p.add_argument("folder")
    p.set_defaults(func=lambda a: tabelaResultadosModelo(_collect_files(a.folder), a.spreadsheet))

    p = sub.add_parser("table-ga", parents=[spreadsheet_parser],
                        help="Detailed per-instance GA table, averaged across run dirs "
                             "(finished/unfinished tasks, switches, result, time; no Best Bound)")
    p.add_argument("dirs", nargs="+", help="Run directories, each containing <subdir>/<instance files>")
    p.add_argument("--subdir", default="MyInstancesSameToolSets")
    p.set_defaults(func=lambda a: tabelaDetalhadaGA(a.dirs, a.subdir, a.spreadsheet))

    p = sub.add_parser("table-pt", parents=[spreadsheet_parser], help="PT results tables (per-instance means, then gap/std stats)")
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

    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
