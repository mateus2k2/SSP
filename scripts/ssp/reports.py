"""Parse a mainCpp solution report (written by SSP::evaluateReport / KTNSReport,
SSP::practitioner and SSP::modelo -- all share this layout):

    <jobsFile>;<toolsFile>
    <horizon in days>;<unsupervised start>;<minutes per day>
    Machine: 0
    <job>;<op>;<start>;<end>;<priority>;<tool>,<tool>,...,
    ...
    Machine: 1
    ...
    END
    <key>: <value>          (fineshedJobsCount, switchs, Final Solution, ...)

Every time in a report is in minutes, except the horizon on line 2, which is
in days (horizon minutes = days x minutes-per-day). "unsupervised start" is the
minute of the day the unsupervised period begins; == minutes per day means the
instance has no unsupervised period (the Beezao .PMTC set).

Times are minutes on that machine's own timeline (every machine starts at 0).
The magazine column lists the tools loaded while the operation runs.

The footer's "Time" is milliseconds for GA/PT/practitioner and seconds for the
Gurobi model (see ssp/results.py, which divides accordingly).
"""
import re


def to_camel_case(s):
    """'Final Solution' -> 'finalSolution'; single words are kept as-is
    ('Time', 'PTL'), so GA/PT's 'Final Solution' and the practitioner's
    'finalSolution' end up under the same key."""
    if not re.search(r"[\s_-]", s):
        return s
    words = re.split(r"[\s_-]+", s)
    return words[0].lower() + "".join(w.capitalize() for w in words[1:])


def _parse_operation(line):
    fields = line.split(";")
    magazine = [int(t) for t in (fields[5] if len(fields) > 5 else "").split(",") if t.strip()]
    return {
        "job": int(fields[0]),
        "operation": int(fields[1]),
        "start": int(fields[2]),
        "end": int(fields[3]),
        "priority": int(fields[4]),
        "magazine": magazine,
    }


def parseReport(file_path):
    """-> (planejamento, machines, endInfo)

    planejamento: planingHorizon, unsupervised, timescale, jobsFileName, toolSetFileName
    machines:     list (one per "Machine:" section, in order) of operation dicts
    endInfo:      footer values, keys camelCased (see to_camel_case)
    """
    with open(file_path) as f:
        lines = f.read().splitlines()
    if "END" not in lines:
        raise ValueError(f"{file_path}: no END marker (incomplete or not a report)")
    end = lines.index("END")

    files = lines[0].split(";")
    horizon, unsupervised, timescale = (int(v) for v in lines[1].split(";")[:3])
    planejamento = {
        "planingHorizon": horizon,
        "unsupervised": unsupervised,
        "timescale": timescale,
        "jobsFileName": files[0],
        "toolSetFileName": files[1] if len(files) > 1 else "",
    }

    machines = []
    for line in lines[2:end]:
        if line.startswith("Machine:"):
            machines.append([])
        elif line.strip():
            machines[-1].append(_parse_operation(line))

    endInfo = {}
    for item in lines[end + 1:]:
        if ":" not in item:
            continue
        key, value = item.replace(";", "").split(":", 1)
        endInfo[to_camel_case(key.strip())] = float(value.strip())

    return planejamento, machines, endInfo
