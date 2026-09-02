import csv
import os
import re
import statistics

from natsort import natsorted

import uteis.reportParser as rp

# ---------------------------------------------------------------------------------------------------
# Analises for the base datasets: BaseGA1, BasePT1, BaseModelo1, BaseHeuristica
# ---------------------------------------------------------------------------------------------------

OUTPUT_DIR = "/workspaces/IC/SSP/output"
TOTAL_PTL = 500

# These Base* instances don't encode a priority rate in the file name (unlike the older
# n=...,p=...,r=... dataset), and the whole dataset was generated at a single fixed
# priority level.
PRIORITY_RATE = 0.5

INSTANCE_NAME_RE = re.compile(r"^(?P<stem>.+_n=(?P<n>\d+),r=(?P<r>[\d.]+),t=\d+,v\d+)$")


def parseInstanceName(fileName):
    stem = os.path.splitext(fileName)[0]
    match = INSTANCE_NAME_RE.match(stem)
    if not match:
        raise ValueError(f"Unrecognized instance file name: {fileName}")
    return stem, int(match.group("n")), float(match.group("r"))


def totalUnfinished(endInfo, n):
    return endInfo.get("totalUnfineshed", n - endInfo["fineshedJobsCount"])


def formatNumber(value):
    if value == "":
        return ""
    return f"{value:.2f}".replace(".", ",")


def writeCsv(outPath, header, rows):
    formattedRows = [[formatNumber(value) for value in row] for row in rows]
    with open(outPath, "w", newline="") as csvFile:
        writer = csv.writer(csvFile, delimiter=";")
        writer.writerow(header)
        writer.writerows(formattedRows)
    print(f"Wrote {len(rows)} rows to {outPath}")


# ---------------------------------------------------------------------------------------------------
# PT / GA (multiple runs per instance, in numbered subfolders)
# ---------------------------------------------------------------------------------------------------

def analiseMultiRun(folder, outName):
    runDirs = natsorted(
        d for d in os.listdir(folder) if os.path.isdir(os.path.join(folder, d))
    )
    fileNames = natsorted(
        f for f in os.listdir(os.path.join(folder, runDirs[0])) if f.endswith(".txt")
    )

    header = [
        "n", "p", "r",
        "Tarefas  finalizadas", "Tarefas  prioritárias  não finalizadas",
        "Total Tarefas  não finalizadas", "Instâncias  de troca", "Trocas de  ferramentas",
        "Media das Melhores  Soluções iniciais", "Melhor solução S*", "Médio da solução S",
        "Desvio Padrão  Solução σ", "Tempo de  execução (s)", "Convergencia",
        "% gap entre S0 e S*",
    ]

    rows = []
    for fileName in fileNames:
        _, n, r = parseInstanceName(fileName)

        fineshedJobsCountAcc = []
        unfineshedPriorityCountAcc = []
        totalUnfinishedAcc = []
        switchsInstancesAcc = []
        switchsAcc = []
        finalSolutionAcc = []
        timeAcc = []
        ptlAcc = []
        bestInitialAcc = []

        for runDir in runDirs:
            filePath = os.path.join(folder, runDir, fileName)
            if not os.path.isfile(filePath) or os.path.getsize(filePath) == 0:
                continue

            _, _, endInfo = rp.parseReport(filePath)

            fineshedJobsCountAcc.append(endInfo["fineshedJobsCount"])
            unfineshedPriorityCountAcc.append(endInfo["unfineshedPriorityCount"])
            totalUnfinishedAcc.append(totalUnfinished(endInfo, n))
            switchsInstancesAcc.append(endInfo["switchsInstances"])
            switchsAcc.append(endInfo["switchs"])
            finalSolutionAcc.append(endInfo["finalSolution"])
            timeAcc.append(endInfo["Time"] / 1000)
            if "PTL" in endInfo:
                ptlAcc.append(endInfo["PTL"])
            if "bestInitial" in endInfo:
                bestInitialAcc.append(endInfo["bestInitial"])

        if not fineshedJobsCountAcc:
            continue

        sStar = max(finalSolutionAcc)
        sMean = statistics.mean(finalSolutionAcc)
        sStdPercent = (
            statistics.stdev(finalSolutionAcc) / sMean * 100
            if len(finalSolutionAcc) > 1 and sMean
            else 0
        )
        bestInitialMean = statistics.mean(bestInitialAcc) if bestInitialAcc else ""
        gap = (sStar - bestInitialMean) / sStar * 100 if bestInitialAcc and sStar else ""
        convergencia = (
            statistics.mean(ptlAcc) / TOTAL_PTL * 100 if ptlAcc else ""
        )

        rows.append([
            n, PRIORITY_RATE, r,
            statistics.mean(fineshedJobsCountAcc),
            statistics.mean(unfineshedPriorityCountAcc),
            statistics.mean(totalUnfinishedAcc),
            statistics.mean(switchsInstancesAcc),
            statistics.mean(switchsAcc),
            bestInitialMean,
            sStar,
            sMean,
            sStdPercent,
            statistics.mean(timeAcc),
            convergencia,
            gap,
        ])

    writeCsv(os.path.join(OUTPUT_DIR, outName), header, rows)


# ---------------------------------------------------------------------------------------------------
# Practitioner (BaseHeuristica) - single deterministic run per instance
# ---------------------------------------------------------------------------------------------------

def analisePractitioner(folder, outName):
    fileNames = natsorted(f for f in os.listdir(folder) if f.endswith(".txt"))

    header = [
        "n", "p", "r",
        "Tarefas  finalizadas", "Tarefas  prioritárias  não finalizadas",
        "Total Tarefas  não finalizadas", "Instâncias  de troca", "Trocas de  ferramentas",
        "Resultado",
    ]

    rows = []
    for fileName in fileNames:
        filePath = os.path.join(folder, fileName)
        if os.path.getsize(filePath) == 0:
            continue

        _, n, r = parseInstanceName(fileName)
        _, _, endInfo = rp.parseReport(filePath)

        rows.append([
            n, PRIORITY_RATE, r,
            endInfo["fineshedJobsCount"],
            endInfo["unfineshedPriorityCount"],
            totalUnfinished(endInfo, n),
            endInfo["switchsInstances"],
            endInfo["switchs"],
            endInfo["finalSolution"],
        ])

    writeCsv(os.path.join(OUTPUT_DIR, outName), header, rows)


# ---------------------------------------------------------------------------------------------------
# Modelo (BaseModelo1) - single run per instance, report lives in the .csv file
# (the .txt file of the same name is left empty by the solver run)
# ---------------------------------------------------------------------------------------------------

def analiseModelo(folder, outName):
    fileNames = natsorted(f for f in os.listdir(folder) if f.endswith(".csv"))

    header = [
        "n", "p", "r",
        "Tarefas  finalizadas", "Tarefas  prioritárias  não finalizadas",
        "Total Tarefas  não finalizadas", "Instâncias  de troca", "Trocas de  ferramentas",
        "Best Bound", "Resultado", "Tempo",
    ]

    rows = []
    for fileName in fileNames:
        filePath = os.path.join(folder, fileName)
        if os.path.getsize(filePath) == 0:
            continue

        _, n, r = parseInstanceName(fileName)
        _, _, endInfo = rp.parseReport(filePath)

        rows.append([
            n, PRIORITY_RATE, r,
            endInfo["fineshedJobsCount"],
            endInfo["unfineshedPriorityCount"],
            totalUnfinished(endInfo, n),
            endInfo["switchsInstances"],
            endInfo["switchs"],
            endInfo["bestBound"],
            endInfo["finalSolution"],
            endInfo["Time"],
        ])

    writeCsv(os.path.join(OUTPUT_DIR, outName), header, rows)


def main():
    analiseMultiRun(os.path.join(OUTPUT_DIR, "BasePT1"), "BasePT1_analise.csv")
    analiseMultiRun(os.path.join(OUTPUT_DIR, "BaseGA1"), "BaseGA1_analise.csv")
    analisePractitioner(os.path.join(OUTPUT_DIR, "BaseHeuristica"), "BaseHeuristica_analise.csv")
    analiseModelo(os.path.join(OUTPUT_DIR, "BaseModelo1"), "BaseModelo1_analise.csv")


if __name__ == "__main__":
    main()
