"""Load an SSP-USPrC instance in any of the repo's four on-disk formats.

Every loader returns an `Instance` whose tool ids are the same ids mainCpp
prints in a report's magazine column, so report and instance can be compared
directly:

  legacy CSV    input/MyInstances*ToolSets/*.csv  jobs CSV + sibling .dat
                (CAPACITY/MACHINES/DAYS/UNSUPERVISED_MINUTS) + shared toolset
                file input/Processed/ToolSetInt.csv (real tool ids)
  BaseInstances input/BaseInstances/<case>/*.txt   "CAPACITY MACHINES DAYS
                UNSUPERVISED", blank line, "job op pt priority t,t,t"
  Consolidated  input/Consolidated/*/*.txt        title, capacity, machines,
                days, unsupervised, blank line, "job op priority pt bits..."
  Beezao        input/BeezaoRaw/*/*.PMTC          "machines jobs tools
                capacity", switching time, processing times, tools x jobs
                0/1 matrix. No horizon/unsupervised/priority in the file:
                every job is a single priority operation and the horizon
                comes from the report header.
"""
import csv
import os
import re
from dataclasses import dataclass, field
from functools import lru_cache

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))


@dataclass(frozen=True)
class Operation:
    job: int
    op: int
    processing_time: int
    priority: int
    tools: frozenset


@dataclass
class Instance:
    path: str
    fmt: str
    capacity: int
    machines: int
    days: object = None          # planning horizon in days (None: not in file)
    unsupervised: object = None  # minute of the day the unsupervised period starts
    ops: dict = field(default_factory=dict)   # (job, op) -> Operation
    warnings: list = field(default_factory=list)  # malformed cells, read like mainCpp does

    @property
    def n_ops(self):
        return len(self.ops)

    @property
    def n_priority(self):
        return sum(o.priority for o in self.ops.values())

    def reentrant_jobs(self):
        return sorted({j for (j, k) in self.ops if k == 1 and (j, 0) in self.ops})


def resolve_path(path, root=REPO_ROOT):
    """Map a path as written in a report header to a file that exists now.

    Reports store the path relative to where mainCpp ran (the repo root), and
    BaseInstances were later moved from `BaseInstances/6M1_n=...txt` into
    `BaseInstances/6M1/n=...txt` (contents verified identical)."""
    p = path.strip()
    candidates = [p, os.path.join(root, p)]
    head, base = os.path.split(p)
    if os.path.basename(head) == "BaseInstances" and "_" in base:
        case, rest = base.split("_", 1)
        candidates.append(os.path.join(root, head, case, rest))
    for c in candidates:
        if os.path.isfile(c):
            return os.path.normpath(c)
    raise FileNotFoundError(f"instance not found: {path}")


def _stoi(text, inst, where):
    """Read an int the way mainCpp's std::stoi does: leading digits only.
    Anything after them is ignored by the solver, so it is recorded as a
    warning instead of failing (e.g. a stray '1z' priority cell)."""
    t = text.strip()
    m = re.match(r"[+-]?\d+", t)
    if not m:
        raise ValueError(f"{where}: not a number: {text!r}")
    if m.group(0) != t:
        inst.warnings.append(f"{where}: {text!r} read as {int(m.group(0))}")
    return int(m.group(0))


@lru_cache(maxsize=4)
def _toolset_table(path):
    table = {}
    with open(path, newline="") as f:
        for row in csv.reader(f, delimiter=";"):
            if not row or not row[0].strip():
                continue
            table[int(row[0])] = frozenset(int(v) for v in row[1:] if v.strip())
    return table


def _load_legacy_csv(path):
    dat = path[:-4] + ".dat"
    params = {}
    with open(dat) as f:
        for line in f:
            parts = line.split()
            if len(parts) == 2:
                params[parts[0]] = int(parts[1])
    toolsets = _toolset_table(os.path.join(REPO_ROOT, "input", "Processed", "ToolSetInt.csv"))
    inst = Instance(path, "legacy-csv", params["CAPACITY"], params["MACHINES"],
                    params["DAYS"], params["UNSUPERVISED_MINUTS"])
    with open(path, newline="", encoding="utf-8-sig") as f:
        for line_no, row in enumerate(csv.DictReader(f, delimiter=";"), start=2):
            v = {k: _stoi(row[k], inst, f"line {line_no} {k}")
                 for k in ("Job", "Operation", "ToolSet", "Processing Time", "Priority")}
            j, k = v["Job"], v["Operation"]
            inst.ops[(j, k)] = Operation(j, k, v["Processing Time"], v["Priority"], toolsets[v["ToolSet"]])
    return inst


def _load_base(path):
    with open(path) as f:
        capacity, machines, days, unsup = map(int, f.readline().split())
        inst = Instance(path, "base", capacity, machines, days, unsup)
        for line in f:
            parts = line.split()
            if len(parts) < 5:
                continue
            j, k, pt, prio = map(int, parts[:4])
            tools = frozenset(int(t) for t in parts[4].split(",") if t)
            inst.ops[(j, k)] = Operation(j, k, pt, prio, tools)
    return inst


def _load_consolidated(path):
    with open(path) as f:
        f.readline()  # informational title
        capacity, machines, days, unsup = (int(f.readline()) for _ in range(4))
        inst = Instance(path, "consolidated", capacity, machines, days, unsup)
        for line in f:
            parts = line.split()
            if len(parts) < 4:
                continue
            j, k, prio, pt = map(int, parts[:4])
            tools = frozenset(i for i, b in enumerate(parts[4:]) if b == "1")
            inst.ops[(j, k)] = Operation(j, k, pt, prio, tools)
    return inst


def _load_beezao(path):
    with open(path) as f:
        tokens = f.read().split()
    machines, jobs, tools, capacity = map(int, tokens[:4])
    # tokens[4] is the switching time (not modelled by mainCpp)
    pts = list(map(int, tokens[5:5 + jobs]))
    matrix = tokens[5 + jobs:5 + jobs + tools * jobs]
    inst = Instance(path, "beezao", capacity, machines)
    for j in range(jobs):
        ts = frozenset(t for t in range(tools) if matrix[t * jobs + j] == "1")
        inst.ops[(j, 0)] = Operation(j, 0, pts[j], 1, ts)
    return inst


@lru_cache(maxsize=256)
def load_instance(path):
    """Load and cache an instance, dispatching on the same path conventions
    mainCpp uses (see SSP::SSP in src/SSP.cpp)."""
    path = resolve_path(path)
    if path.endswith(".PMTC") or "Beezao" in path:
        return _load_beezao(path)
    if "BaseInstances" in path:
        return _load_base(path)
    if "Consolidated" in path:
        return _load_consolidated(path)
    return _load_legacy_csv(path)
