"""Validate a mainCpp solution report against its instance.

Checks every constraint of the SSP-USPrC (see the thesis, chap. 3 and 4) on the
schedule the report prints, then recomputes every footer counter and the
objective from that schedule and compares them with what the report claims.

Horizon, unsupervised start and day length are taken from the report's own
header (line 2): different code versions wrote different values there (e.g.
the Beezao PT runs use horizon=1 day, unsupervised from minute 1068), and a
report can only be judged against the rules it was produced under.

Severity:
  error    the schedule is infeasible, or the report states a wrong value
  warning  a reporting quirk that does not change feasibility or the value

Known sources of findings in reports written by mainCpp before the September
2026 fixes (e.g. every run in output-final/; checked against that code):
  duration-reentrant-swap  KTNSReport / modelo.cpp print a grouped pair with
                           processingTimes = [p1, p0] (loadData.cpp groupJobs)
  duration-reentrant-sum   different-toolset mode: groupJobs adds op 1's time
                           to op 0 without grouping them, so op 0 is charged
                           p0+p1 and op 1 p1 again
  reentrant-*              different-toolset mode: expandSolution keeps a pair
                           adjacent, but splitSolutionIntoMachines cuts the
                           sequence by count and can cut it between op 0 and
                           op 1 (at most one pair per machine boundary);
                           the practitioner and the Gurobi model (constraint
                           (9) is a plain time precedence) do not impose the
                           strong chain on ungrouped pairs at all
  time-truncation          modelo.cpp writes Gurobi's double s/e through int
                           parameters, e.g. 1906.9999999 is printed as 1906
  counter-unfinished-priority, objective
                           practitioner.cpp printed the FINISHED priority count
  tools-missing            KTNS loads at most `capacity` tools, so an operation
                           needing more was "processed" without all its tools
"""
import os
from dataclasses import dataclass, field

from . import instances
from . import reports as rp

# Objective weights: FO = profit*finished - switch*switches
#                         - instance*switchInstances - priority*unfinishedPriority
COST_PRESETS = {
    "default": dict(profit=30, switch=1, instance=10, priority=30),  # thesis / Holanda et al.
    "beezao":  dict(profit=0,  switch=1, instance=0,  priority=30),  # runAuto.sh "beezao" mode
}

CHECKS = {
    "report-parse":            "report could not be parsed",
    "instance":                "instance file missing or unreadable",
    "machines":                "more machines used than the instance has",
    "operation-unknown":       "operation not in the instance",
    "operation-duplicate":     "operation scheduled more than once",
    "priority-label":          "priority printed differs from the instance",
    "duration":                "end - start differs from the processing time",
    "duration-reentrant-swap": "reentrant pair: op0/op1 durations printed swapped (total correct)",
    "duration-reentrant-sum":  "reentrant op 0 charged op0+op1 time while op 1 is charged again (time wasted)",
    "instance-data":           "malformed instance cell (read like mainCpp's stoi)",
    "instance-over-capacity":  "instance operation needs more tools than the magazine holds (can never be processed)",
    "overlap":                 "operation starts before the previous one on the machine ends",
    "time-truncation":         "Gurobi times off by 1 minute: modelo.cpp truncates double s/e to int",
    "horizon":                 "printed operation ends after the planning horizon (counted as unfinished)",
    "tools-missing":           "magazine lacks a tool the operation needs",
    "capacity":                "magazine holds more tools than its capacity",
    "unsupervised-switch":     "tools switched with no supervised moment available",
    "unsupervised-boundary":   "tools switched exactly at the minute the unsupervised period starts",
    "reentrant-precedence":    "op 1 scheduled without op 0",
    "reentrant-split":         "op 0 and op 1 of a job on different machines",
    "reentrant-order":         "op 1 not immediately after op 0 on its machine",
    "counter-finished":        "reported finished operations differ from the schedule",
    "counter-switches":        "reported tool switches differ from the schedule",
    "counter-switch-instances": "reported switch instances differ from the schedule",
    "counter-unfinished-priority": "reported unfinished priority ops differ from the schedule",
    "counter-total-unfinished": "reported unfinished operations differ from the schedule",
    "objective":               "reported objective differs from the value of the schedule",
    "objective-formula":       "reported objective differs from the formula applied to the reported counters",
    "best-bound":              "Gurobi best bound below the solution value (maximization)",
    "best-initial":            "PT best initial solution better than the final solution",
}


@dataclass
class Issue:
    check: str
    severity: str
    message: str
    machine: object = None
    job: object = None
    op: object = None


@dataclass
class Result:
    report: str
    instance: str = ""
    method: str = ""
    costs: str = ""
    issues: list = field(default_factory=list)
    recomputed: dict = field(default_factory=dict)
    reported: dict = field(default_factory=dict)

    @property
    def ok(self):
        return not any(i.severity == "error" for i in self.issues)

    def add(self, check, severity, message, machine=None, job=None, op=None):
        self.issues.append(Issue(check, severity, message, machine, job, op))


def detect_method(endInfo, path=""):
    p = path.lower()
    for hint, method in (("modelo", "modelo"), ("practitioner", "practitioner"), ("heuristica", "practitioner"),
                         ("pt-ssp", "pt"), ("genetic", "ga"), ("/ga", "ga")):
        if hint in p:
            return method
    if "bestBound" in endInfo:
        return "modelo"
    if "PTL" in endInfo:
        return "pt"
    if "criticalMachineTime" in endInfo:
        return "ga"
    return "practitioner"


def objective(costs, finished, switches, instances_, unfinished_priority):
    c = COST_PRESETS[costs] if isinstance(costs, str) else costs
    return (c["profit"] * finished - c["switch"] * switches
            - c["instance"] * instances_ - c["priority"] * unfinished_priority)


def _supervised_moment(t0, t1, unsupervised, day, closed=False):
    """Is there an instant t in [t0, t1] with t % day < unsupervised?
    unsupervised >= day means the instance has no unsupervised period.
    closed=True also accepts t % day == unsupervised, the boundary the Gurobi
    model allows (its constraint (16) is h(jk) <= tU, inclusive)."""
    u = unsupervised + (1 if closed else 0)
    if u >= day or t1 - t0 >= day:
        return True
    if t0 % day < u or t1 % day < u:
        return True
    return t0 // day != t1 // day and u > 0  # crosses midnight


def validate_report(path, costs="auto"):
    res = Result(report=path)
    try:
        plan, machines, end = rp.parseReport(path)
    except Exception as e:  # noqa: BLE001 - any parse failure is a finding
        res.add("report-parse", "error", str(e))
        return res
    res.reported = end
    res.method = detect_method(end, path)
    try:
        inst = instances.load_instance(plan["jobsFileName"])
    except Exception as e:  # noqa: BLE001
        res.add("instance", "error", f"{plan['jobsFileName']}: {e}")
        return res
    res.instance = os.path.relpath(inst.path, instances.REPO_ROOT)
    for w in inst.warnings:
        res.add("instance-data", "warning", w)
    for (j, k), spec in sorted(inst.ops.items()):
        if len(spec.tools) > inst.capacity:
            res.add("instance-over-capacity", "warning",
                    f"needs {len(spec.tools)} tools, capacity {inst.capacity}", job=j, op=k)
    # modelo.cpp truncates Gurobi's times: tolerate (and report) 1 minute
    tol = 1 if res.method == "modelo" else 0
    if costs == "auto":
        costs = "beezao" if inst.fmt == "beezao" else "default"
    res.costs = costs

    day = plan["timescale"]
    horizon = plan["planingHorizon"] * day
    unsupervised = plan["unsupervised"]

    used_machines = sum(1 for m in machines if m)
    if used_machines > inst.machines:
        res.add("machines", "error", f"{used_machines} machines used, instance has {inst.machines}")

    where = {}          # (job, op) -> (machine, index in machine)
    finished = set()
    switches = switch_instances = 0
    for m, ops in enumerate(machines):
        prev = None
        for i, o in enumerate(ops):
            key = (o["job"], o["operation"])
            spec = inst.ops.get(key)
            if spec is None:
                res.add("operation-unknown", "error", f"({key[0]},{key[1]}) not in instance", m, *key)
                prev = o
                continue
            if key in where:
                res.add("operation-duplicate", "error", f"({key[0]},{key[1]}) also on machine {where[key][0]}", m, *key)
            where[key] = (m, i)

            if o["priority"] != spec.priority:
                res.add("priority-label", "error", f"printed {o['priority']}, instance {spec.priority}", m, *key)
            gap = o["start"] - (prev["end"] if prev is not None else 0)
            if gap < -tol:
                res.add("overlap", "error", f"starts {o['start']}, previous ends {prev['end'] if prev else 0}", m, *key)
            elif gap < 0:
                res.add("time-truncation", "warning", f"starts {o['start']}, previous ends {prev['end']}", m, *key)
            if o["end"] > horizon:
                res.add("horizon", "warning", f"ends {o['end']} > horizon {horizon}", m, *key)
            else:
                finished.add(key)

            mag = set(o["magazine"])
            missing = spec.tools - mag
            if missing:
                res.add("tools-missing", "error", f"{len(missing)} tools missing, e.g. {sorted(missing)[:5]}", m, *key)
            if len(mag) > inst.capacity:
                res.add("capacity", "error", f"{len(mag)} tools > capacity {inst.capacity}", m, *key)

            if prev is not None:
                added = mag - set(prev["magazine"])
                if added:
                    switches += len(added)
                    switch_instances += 1
                    t0, t1 = prev["end"], o["start"]
                    if not _supervised_moment(t0, t1, unsupervised, day):
                        closed = _supervised_moment(t0, t1, unsupervised, day, closed=True)
                        res.add("unsupervised-boundary" if closed else "unsupervised-switch",
                                "warning" if closed else "error",
                                f"{len(added)} tools added between {prev['end']} and {o['start']} "
                                f"(unsupervised from minute {unsupervised} of each {day}-minute day)", m, *key)
            prev = o

    def op_at(pos):
        return machines[pos[0]][pos[1]]

    def duration(pos):
        return op_at(pos)["end"] - op_at(pos)["start"]

    def close(a, b):
        return abs(a - b) <= tol

    # durations, with the reentrant op0/op1 swap told apart from real errors
    explained = set()
    for j in inst.reentrant_jobs():
        a, b = where.get((j, 0)), where.get((j, 1))
        p0, p1 = inst.ops[(j, 0)].processing_time, inst.ops[(j, 1)].processing_time
        if a and p1 > 0 and close(duration(a), p0 + p1):
            explained.add((j, 0))
            res.add("duration-reentrant-sum", "warning", f"op 0 takes {duration(a)} = {p0}+{p1}", a[0], j, 0)
        if a and b and a[0] == b[0] and b[1] == a[1] + 1:
            if p0 != p1 and close(duration(a), p1) and close(duration(b), p0):
                explained |= {(j, 0), (j, 1)}
                res.add("duration-reentrant-swap", "warning", f"printed {duration(a)}/{duration(b)}, instance {p0}/{p1}",
                        a[0], j, 0)
    for key, pos in where.items():
        d, p = duration(pos), inst.ops[key].processing_time
        if not close(d, p):
            if key not in explained:
                res.add("duration", "error", f"{d} vs {p}", pos[0], *key)
        elif d != p and key not in explained:
            res.add("time-truncation", "warning", f"duration {d} vs {p}", pos[0], *key)

    # strong chain between the two operations of a reentrant job
    def cut_by_split(a, b):
        """op 1 opens machine m+1 and op 0 (if printed) closes machine m: the
        GA/PT decoder cut the pair when splitting the sequence by count."""
        return b[1] == 0 and b[0] > 0 and (a is None or (a[0] == b[0] - 1 and a[1] == len(machines[a[0]]) - 1))

    for j in inst.reentrant_jobs():
        a, b = where.get((j, 0)), where.get((j, 1))
        note = " (pair cut by the machine split)" if b and cut_by_split(a, b) else ""
        if b and not a:
            res.add("reentrant-precedence", "error", "op 1 scheduled, op 0 not" + note, b[0], j, 1)
        elif a and b:
            if a[0] != b[0]:
                timing = "after" if op_at(b)["start"] >= op_at(a)["end"] - tol else "BEFORE"
                res.add("reentrant-split", "error",
                        f"op 0 on machine {a[0]}, op 1 on machine {b[0]}, starting {timing} op 0 ends" + note, a[0], j)
            elif b[1] != a[1] + 1:
                res.add("reentrant-order", "error", f"op 0 at position {a[1]}, op 1 at {b[1]}", a[0], j)

    unfinished_priority = sum(o.priority for k, o in inst.ops.items() if k not in finished)
    rec = dict(finished=len(finished), switches=switches, switchInstances=switch_instances,
               unfinishedPriority=unfinished_priority, totalUnfinished=inst.n_ops - len(finished))
    rec["objective"] = objective(costs, rec["finished"], switches, switch_instances, unfinished_priority)
    res.recomputed = rec

    for check, rep_key, rec_key in [
        ("counter-finished", "fineshedJobsCount", "finished"),
        ("counter-switches", "switchs", "switches"),
        ("counter-switch-instances", "switchsInstances", "switchInstances"),
        ("counter-unfinished-priority", "unfineshedPriorityCount", "unfinishedPriority"),
        ("counter-total-unfinished", "totalUnfineshed", "totalUnfinished"),
    ]:
        if rep_key in end and end[rep_key] != rec[rec_key]:
            res.add(check, "error", f"reported {end[rep_key]:g}, schedule gives {rec[rec_key]}")

    fo = end.get("finalSolution")
    if fo is not None:
        if abs(fo - rec["objective"]) > 1e-6:
            res.add("objective", "error", f"reported {fo:g}, schedule is worth {rec['objective']} ({costs} costs)")
        if all(k in end for k in ("fineshedJobsCount", "switchs", "switchsInstances", "unfineshedPriorityCount")):
            formula = objective(costs, end["fineshedJobsCount"], end["switchs"],
                                end["switchsInstances"], end["unfineshedPriorityCount"])
            if abs(fo - formula) > 1e-6:
                res.add("objective-formula", "error", f"reported {fo:g}, formula on reported counters {formula:g}")
        if "bestBound" in end and end["bestBound"] < fo - 1e-6:
            res.add("best-bound", "error", f"best bound {end['bestBound']:g} < solution {fo:g}")
        if "bestInitial" in end and end["bestInitial"] > fo + 1e-6:
            res.add("best-initial", "warning", f"best initial {end['bestInitial']:g} > final {fo:g}")
    return res


def is_report(path):
    """Cheap sniff: a report's second line is 'H;U;DAY' and it has an END line."""
    try:
        with open(path) as f:
            f.readline()
            second = f.readline().strip().split(";")
            if len(second) != 3 or not all(s.isdigit() for s in second):
                return False
            return any(line.strip() == "END" for line in f)
    except (OSError, UnicodeDecodeError):
        return False
