"""Turn the runs under output-final/ into the rows of the results spreadsheet.

Every report is first checked by uteis.validador, so a row can be built from
either the counters the solver printed in the report footer ("reported", what
the original Google Sheet used) or the ones the validator recomputed from the
printed schedule ("recomputed"), and each row carries its validation status.

Layout of output-final/ this module expects (see output-final/README.md):

  <experiment>/<method folder>/run-01 ... run-10/<report>   10-run methods (PT, GA)
  <experiment>/<method folder>/<report>                     single-run methods
"""
import os
import re
import statistics
from collections import Counter
from concurrent.futures import ProcessPoolExecutor
from dataclasses import dataclass, field

from . import validador as vd

RUN_DIR = re.compile(r"^(run-)?\d+$")
GRID_NAME = re.compile(r"n=(\d+),p=([\d.]+),r=([\d.]+)")
BASE_NAME = re.compile(r"^(\w+?)_n=(\d+),r=([\d.]+)")
BASE_CASES = ["2M1", "6M1", "6M2"]  # sheet order of the base (Dang) instance classes
BASE_P = 0.5                        # every base instance class was generated with p = 0.5
BEEZAO_NAME = re.compile(r"^instance\D*(\d+)")

COUNTERS = ("finished", "unfinishedPriority", "totalUnfinished", "switchInstances", "switches", "objective")
COUNTER_HEADERS = ["Tarefas finalizadas", "Tarefas prioritárias não finalizadas",
                   "Total Tarefas não finalizadas", "Instâncias de troca", "Trocas de ferramentas"]
VALIDATION_HEADER = "Validação"


@dataclass
class Grid:
    """One spreadsheet tab of n/p/r-indexed instances (Same, Different, Dang)."""
    tab: str
    folder: str
    naming: str          # "grid" (n=..,p=..,r=..) or "base" (2M1_n=..,r=..)
    ptl_limit: int       # PT's --PTL: the convergence column is mean(PTL) / ptl_limit
    blocks: tuple = (("PT", "PT-SSP-USPrC"), ("Practitioner", "Practitioner-single-run"),
                     ("MODELO", "Modelo-gurobi-single-run"), ("GA", "AG-genetic-algorithm"))


@dataclass
class Beezao:
    """One spreadsheet tab of Beezao .PMTC instances, one row per instance."""
    tab: str
    folder: str
    title: str


# PTL limits: the reports' PTL counter is 0-based and tops out at 599 (same,
# different) and 499 (base) -- the base runs used --PTL 500.
TABS = [
    Grid("Same Toolset", "same-toolset", "grid", 600),
    Grid("Different Toolset", "different-toolset", "grid", 600),
    Grid("Dang reproduzidas", "base-instances", "base", 500),
    Beezao("IPMTC II PT", "beezao-IPMTC-II/PT-SSP-USPrC", "PT"),
    Beezao("IPMTC II Practitioner", "beezao-IPMTC-II/HP-practitioner-single-run", "Practitioner"),
]


@dataclass
class Block:
    title: str
    headers: list
    rows: list = field(default_factory=list)      # list of lists (values; None = empty cell)
    keys: list = field(default_factory=list)      # row key (for joins/comparison), one per row
    status: list = field(default_factory=list)    # per row: list of vd.Result of its reports
    source: str = ""                              # folder the block was built from


# ---------------------------------------------------------------------------------------------------
# collecting and validating reports
# ---------------------------------------------------------------------------------------------------

def report_files(folder):
    """-> [(run, path)]: every report in a method folder, run = '' for single-run folders."""
    out = []
    subdirs = sorted(d for d in os.listdir(folder) if os.path.isdir(os.path.join(folder, d)))
    runs = [d for d in subdirs if RUN_DIR.match(d)]
    for run in runs or [""]:
        d = os.path.join(folder, run)
        for n in sorted(os.listdir(d)):
            f = os.path.join(d, n)
            if os.path.isfile(f) and not n.endswith((".sol", ".lp", ".md")) and vd.is_report(f):
                out.append((run, f))
    return out


def instance_key(path, naming):
    """Row key of a report, from its file name."""
    name = os.path.basename(path)
    if naming == "grid":
        n, p, r = GRID_NAME.search(name).groups()
        return (int(n), float(p), float(r))
    if naming == "base":
        case, n, r = BASE_NAME.search(name).groups()
        return (BASE_CASES.index(case), int(n), float(r))
    return (int(BEEZAO_NAME.match(name).group(1)), os.path.splitext(name)[0])


def _validate(path):
    return vd.validate_report(path)


def validate_all(paths, jobs=None):
    with ProcessPoolExecutor(max_workers=jobs) as ex:
        return dict(zip(paths, ex.map(_validate, paths, chunksize=16)))


def all_report_paths(root):
    paths = []
    for tab in TABS:
        if isinstance(tab, Grid):
            folders = [os.path.join(root, tab.folder, sub) for _, sub in tab.blocks]
        else:
            folders = [os.path.join(root, tab.folder)]
        for f in folders:
            if os.path.isdir(f):
                paths += [p for _, p in report_files(f)]
    return paths


# ---------------------------------------------------------------------------------------------------
# values
# ---------------------------------------------------------------------------------------------------

def counters(res, mode):
    """The six solution counters of one report, as printed or as recomputed."""
    rec, end = res.recomputed, res.reported
    if not rec:
        raise ValueError(f"{res.report}: could not be validated ({res.issues[0].message})")
    if mode == "recomputed":
        return {k: rec[k] for k in COUNTERS}
    fin = end["fineshedJobsCount"]
    n_ops = rec["finished"] + rec["totalUnfinished"]
    return dict(finished=fin, unfinishedPriority=end["unfineshedPriorityCount"],
                # the Gurobi report has no totalUnfineshed line
                totalUnfinished=end.get("totalUnfineshed", n_ops - fin),
                switchInstances=end["switchsInstances"], switches=end["switchs"],
                objective=end["finalSolution"])


def mean(xs):
    return statistics.fmean(xs)


def _counter_means(cs):
    return [mean([c[k] for c in cs]) for k in COUNTERS[:5]]


def row_pt(results, mode, ptl_limit):
    cs = [counters(r, mode) for r in results]
    ends = [r.reported for r in results]
    fo = [c["objective"] for c in cs]
    s_star, s_mean = max(fo), mean(fo)
    s0 = mean([e["bestInitial"] for e in ends])
    sigma = statistics.stdev(fo) / abs(s_mean) * 100 if len(fo) > 1 and s_mean else 0.0
    return _counter_means(cs) + [
        s0, s_star, s_mean, sigma,
        mean([e["Time"] for e in ends]) / 1000,
        mean([e["PTL"] for e in ends]) / ptl_limit * 100,
        (s_star - s0) / s_star * 100 if s_star else None,
    ]


def row_practitioner(results, mode):
    c = counters(results[0], mode)
    return _counter_means([c]) + [c["objective"]]


def row_modelo(results, mode):
    c, e = counters(results[0], mode), results[0].reported
    return _counter_means([c]) + [e.get("bestBound"), c["objective"], e.get("Time")]  # Gurobi Time is in s


def row_ga(results, mode):
    cs = [counters(r, mode) for r in results]
    return _counter_means(cs) + ["-", mean([c["objective"] for c in cs]),
                                 mean([r.reported["Time"] for r in results]) / 1000]


def row_beezao(results, mode, multi_run):
    cs = [counters(r, mode) for r in results]
    ends = [r.reported for r in results]
    # Beezao instances have no priority classes (every op is priority): the
    # thesis objective ("FO Real") charges 30 per unfinished op
    fo_real = [vd.objective("default", c["finished"], c["switches"], c["switchInstances"], c["totalUnfinished"])
               for c in cs]
    return [
        mean([c["finished"] for c in cs]), mean([c["totalUnfinished"] for c in cs]),
        mean([c["switches"] for c in cs]), mean([c["switchInstances"] for c in cs]),
        mean([e["bestInitial"] for e in ends]) if multi_run else "-",
        mean([c["objective"] for c in cs]), mean(fo_real),
        mean([e["Time"] for e in ends]) / 1000,
        mean([e["PTL"] for e in ends]) if multi_run else "-",
    ]


HEADERS = {
    "PT": ["n", "p", "r", *COUNTER_HEADERS, "Media das Melhores Soluções iniciais", "Melhor solução S*",
           "Médio da solução S", "Desvio Padrão Solução σ", "Tempo de execução (s)", "Convergencia %",
           "gap entre S0 e S*"],
    "Practitioner": ["n", "p", "r", *COUNTER_HEADERS, "Resultado"],
    "MODELO": ["n", "p", "r", *COUNTER_HEADERS, "Best Bound", "Resultado", "Tempo"],
    "GA": ["n", "p", "r", *COUNTER_HEADERS, "Best Bound", "Resultado", "Tempo"],
    "beezao": ["Instancia", "Tarefas Finalizadas", "Tarefas não finalizadas", "Trocas", "Instancias de troca",
               "Soluções Iniciais", "FO Dang", "FO Real", "Tempo", "PTL"],
}


def status_text(results):
    """Short validation verdict for one row: 'ok' or the failed checks."""
    bad = [r for r in results if not r.ok]
    if not bad:
        return "ok"
    checks = sorted({i.check for r in bad for i in r.issues if i.severity == "error"})
    runs = f"{len(bad)}/{len(results)} execuções" if len(results) > 1 else "erro"
    return f"{runs}: {', '.join(checks)}"


def _grouped(folder, naming, validated):
    groups = {}
    for run, path in report_files(folder):
        groups.setdefault(instance_key(path, naming), []).append(validated[path])
    return dict(sorted(groups.items()))


def build_tab(tab, root, validated, mode):
    """-> list of Block for one tab."""
    blocks = []
    if isinstance(tab, Grid):
        for title, sub in tab.blocks:
            folder = os.path.join(root, tab.folder, sub)
            if not os.path.isdir(folder):
                continue
            blk = Block(title, HEADERS[title] + [VALIDATION_HEADER], source=folder)
            for key, results in _grouped(folder, tab.naming, validated).items():
                if tab.naming == "base":
                    n, p, r = key[1], BASE_P, key[2]
                else:
                    n, p, r = key
                if title == "PT":
                    vals = row_pt(results, mode, tab.ptl_limit)
                elif title == "Practitioner":
                    vals = row_practitioner(results, mode)
                elif title == "MODELO":
                    vals = row_modelo(results, mode)
                else:
                    vals = row_ga(results, mode)
                blk.rows.append([n, p, r, *vals, status_text(results)])
                blk.keys.append((n, p, r))
                blk.status.append(results)
            blocks.append(blk)
    else:
        folder = os.path.join(root, tab.folder)
        blk = Block(tab.title, HEADERS["beezao"] + [VALIDATION_HEADER], source=folder)
        multi = any(run for run, _ in report_files(folder))
        for key, results in _grouped(folder, "beezao", validated).items():
            blk.rows.append([key[1], *row_beezao(results, mode, multi), status_text(results)])
            blk.keys.append(key[1])
            blk.status.append(results)
        blocks.append(blk)
    return blocks


def build(root, mode="recomputed", jobs=None, validated=None):
    """-> (tabs: {tab name: [Block]}, validated: {path: vd.Result})"""
    if validated is None:
        validated = validate_all(all_report_paths(root), jobs)
    return {t.tab: build_tab(t, root, validated, mode) for t in TABS}, validated


def issue_summary(results):
    """-> (errors Counter, warnings Counter) of check ids over some reports."""
    err, warn = Counter(), Counter()
    for r in results:
        for i in r.issues:
            (err if i.severity == "error" else warn)[i.check] += 1
    return err, warn
