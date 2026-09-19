# scripts/

Everything around the C++ solver (`src/`, built into `src/out/mainCpp`): creating
the instances it reads, running it over instance sets, and checking and
summarizing the reports it writes. Run every command from the repo root.

| Folder | Responsibility |
|---|---|
| `instances/` | build and inspect the input data (`input/`) |
| `experiments/` | run `mainCpp` over whole instance sets |
| `results/` | validate reports and turn them into tables/spreadsheets |
| `ssp/` | shared Python package used by the scripts above |
| `deprecated/` | superseded scripts, kept for reference (they do not run as-is) |

Python needs `pandas`, `natsort` and `openpyxl` (installed by the devcontainer).

## instances/

| Script | What it does |
|---|---|
| `generateInstances.py same\|different\|base` | generate the SameToolSets / DiferentToolSets (Consolidated format) and BaseInstances sets |
| `preprocessToolData.py` | rebuild the derived files in `input/Processed/` from the raw job/toolset data |
| `filterCandidates.py` | list raw jobs/toolsets that should be filtered out (oversize, empty, subsets) |
| `countTools.py <folder>` | distinct tools, capacity and over-capacity operations per instance file |
| `printHorizons.sh [folder]` | print the planning horizon (DAYS) of every legacy `.dat` file |

## experiments/

| Script | What it does |
|---|---|
| `runExperiment.sh <out> <same\|diferent\|both\|beezao> <method>` | one report per instance for one method (`ga`, `pt`, `onb`, `modelo`, `practitioner`) |
| `runRepeated.sh <out> ... [repeats]` | `runExperiment.sh` repeated into `<out>/1`, `<out>/2`, ... |

## results/

| Script | What it does |
|---|---|
| `validateRuns.py <paths...>` | check every report against its instance (all SSP-USPrC rules) and recompute its counters and objective |
| `buildSpreadsheet.py` | `output-final/Resultados-SSP-USPrC.xlsx`: the results tables, validated (`--compare` checks against the Google Sheet export) |
| `latexTables.py table-*` | LaTeX (or `--spreadsheet`) rows for one method's runs |
| `meanTimes.sh <folder>` | mean run time per instance across repeated runs |
| `ktnsTrace.py <report>` | magazine trace of one report, to inspect KTNS decisions |
| `gantt/` | browser Gantt chart of a report (`gantt/src/chart.html`) |

## ssp/ (shared package)

| Module | Contents |
|---|---|
| `instances.py` | load an instance in any input format (legacy CSV, BaseInstances, Consolidated, Beezao) |
| `reports.py` | parse a `mainCpp` report |
| `validation.py` | the checks behind `validateRuns.py` |
| `results.py` | aggregation behind `buildSpreadsheet.py` |
| `csvData.py` | pandas loaders for the `;`-separated job/toolset CSVs |
| `processingTimes.py` | sample processing times from the real job data |

Entry scripts add `scripts/` to `sys.path` themselves and `import ssp`; there is no install
step: `python3 scripts/results/validateRuns.py output-final`.
