# output-final — runs behind the thesis LaTeX tables

Copies (not moves) of the runs in `output/` that were checked, value by value,
against the published tables. `output/` is untouched. Run folders are renamed
`run-01` … `run-10` (originals were `1` … `10`); instance filenames are unchanged
so `scripts/results/latexTables.py` and `scripts/results/meanTimes.sh` work on them.

## Spreadsheet and validation

- `Resultados-SSP-USPrC.xlsx` — built by `python3 scripts/results/buildSpreadsheet.py` (or `make planilhaResultados`)
  from the folders below: the tabs of the "Resultados SSP-USPrC" Google Sheet plus LEIA-ME (how each
  column is computed), Validação and Correções. By default the counters/objective are the ones the
  validator recomputes from each printed schedule; `--values reported` uses the report footers instead
  and then reproduces the Google Sheet (`--compare <dir with the sheet's CSV export>`; the only
  differences are 7 corrupted Different Toolset Practitioner cells, e.g. `6,39` for 6390, and
  instanceLarge13/14, missing from the sheet's Practitioner tab).
### Practitioner runs regenerated 2026-09-21

The four practitioner folders were **rerun with the fixed solver** (commit `01b7f547`) and now validate
with 0 errors. The runs that produced the published tables are kept next to them as
`*-single-run-pre-fix/`. What changed against those originals:

| Folder | Objective changed | Switches changed | Finished changed | Why |
|---|---|---|---|---|
| `same-toolset/Practitioner-single-run` | 41 of 42 | 3 | 2 | priority counter; + n=212 v7/v8 over-capacity op, n=15 p=0.75 balancing |
| `different-toolset/Practitioner-single-run` | 42 of 42 | 41 | 33 | priority counter + strong chain + op 1's time no longer spent twice |
| `base-instances/Practitioner-single-run` | 37 of 37 | 0 | 0 | priority counter only |
| `beezao-IPMTC-II/HP-practitioner-single-run` | 1440 of 1440 | 0 | 0 | priority counter only: with the horizon restored to delta* (`58e28b7c`, see FINDINGS.md section 9) every report reproduces the published switches and finished count exactly (instance 931: 196 of 200, 909 switches) |

The GA, PT and Modelo folders still hold their original runs, so `validateRuns.py output-final` still
reports errors for the different-toolset GA (320), PT (382) and Modelo (4) runs, plus everything in the
`-pre-fix` folders.

- `python3 scripts/results/validateRuns.py output-final` checks every report against its instance.
  Findings, all traced to the code. **All are fixed in `src/` since September 2026; the GA, PT and
  Modelo runs here predate the fixes**, so they still show them. Different-toolset and practitioner
  results change with the fixed code. The same-toolset and base GA results also differ on the large
  instances (n >= 209), but for another reason: those runs come from a code version older than the
  repository's recent history, so even the original seed no longer reproduces them (FINDINGS.md
  section 12). Only the three n=212 instances with an over-capacity operation (v6, v7, v8) change
  because of a fix:
  - **Practitioner, every folder**: `src/practitioner.cpp` prints the number of *finished* priority
    operations as `unfineshedPriorityCount` and uses it in `finalSolution`, so the Practitioner
    "prioritárias não finalizadas" / "Resultado" (and Beezao HP "FO Dang") are wrong in the sheet.
    Tool switches and finished counts are right.
  - **Different toolset, PT and GA**: `splitSolutionIntoMachines` splits the sequence by operation
    count and can cut a reentrant pair at a machine boundary (≤ 1 pair per boundary): op 1 opens the
    next machine, sometimes finished while op 0 is not. Strong chain broken in 320/420 GA and 382/420
    PT reports; the op 1 counted without its op 0 inflates those objectives by a median 2.1%.
  - **Different toolset, every method**: `groupJobs` (src/loadData.cpp) adds op 1's time to op 0
    without grouping them, so op 1's time is spent twice (idle time, not infeasible).
  - **Different toolset, Practitioner**: no strong chain at all. **Modelo**: constraint (9) is a plain
    time precedence (weak chain), so split or interleaved pairs appear in all 4 of its reports.
  - **Same toolset n=212 v6/v7/v8**: one operation needs 105 tools, capacity 80. PT/GA never finish
    it; the Practitioner processes it with 80 tools in v7 and v8.
  - Harmless: grouped pairs printed with op0/op1 durations swapped; Modelo times truncated to int
    (1-minute "overlaps"); Modelo allows a switch exactly at minute 720; a `1z` cell in
    `n=75,p=0.24` (read as 1, like `std::stoi`).
  - Base instances, same-toolset PT/GA, Beezao PT: no errors.

## Map: table column -> folder

| Table | Column | Folder here | Copied from |
|---|---|---|---|
| same toolset (`table:comparativasame`) | AG | `same-toolset/AG-genetic-algorithm/` | `output/GATestes/*/MyInstancesSameToolSets` |
| same toolset | PT-SSP-USPrC | `same-toolset/PT-SSP-USPrC/` | `output/sameTesla/*` |
| different toolset (`table:comparativadiff`) | AG | `different-toolset/AG-genetic-algorithm/` | `output/GATestes/*/MyInstancesDiferentToolSets` |
| different toolset | PT-SSP-USPrC | `different-toolset/PT-SSP-USPrC/` | `output/diffTesla/*` |
| Beezao IPMTC-II (`table:resultadosbeezao`) | HP | `beezao-IPMTC-II/HP-practitioner-single-run/` | `output/BeezaoPractitioner` |
| Beezao IPMTC-II | PT-SSP-USPrC | `beezao-IPMTC-II/PT-SSP-USPrC/` | `output/BeezaoPTLarge` |
| sheet "Same Toolset" | Practitioner, MODELO | `same-toolset/{Practitioner,Modelo-gurobi}-single-run/` | `output/{practitioner,modelo}Final/MyInstancesSameToolSets` |
| sheet "Different Toolset" | Practitioner, MODELO | `different-toolset/{Practitioner,Modelo-gurobi}-single-run/` | `output/{practitioner,modelo}Final/MyInstancesDiferentToolSets` |

## How each table value is derived from these runs

- **AG, S**: `Final Solution`. These runs used the hardcoded seed 42, so all 10 are identical.
  (Reruns draw a random seed per run - see `runExperiment.sh`'s `GA_SEED` - so S becomes a
  distribution like PT's, and the current code no longer reproduces these values on the large
  instances: FINDINGS.md section 12.)
- **AG, T**: mean `Time` over the 10 runs, in seconds (`scripts/results/meanTimes.sh <folder>`).
- **PT-SSP-USPrC, S** (same/different toolset): the **best (max) `Final Solution` over the 10 runs**.
- **PT-SSP-USPrC, T**: mean `Time` over the 10 runs.
- **Beezao**: the table's S is the number of **tool switches** (`switchs:` for HP; the "Trocas"
  column of `results_mean.csv` for PT), not `Final Solution`. The table uses instances
  931, 932, 933, 946–948, 952–954, 958–960; these folders hold all 1440 IPMTC-II instances.
  HP is a single run. The Beezao PT runs are 10 runs averaged in `results_mean.csv`.

## Verified against the LaTeX

- same/different AG: 42/42 rows each, S and T.
- same/different PT: spot-checked S (best of 10) and T (mean of 10) on n=15 and n=1236 rows; all exact.
- Beezao HP and PT: instance 931 checked exactly (909 and 554,8); the HP/PT columns' instance set is IPMTC-II ("Large").

## Not included on purpose

- **Beezao AG**: no run matching the table's AG column exists in the repo. A fresh GA re-run
  (`output/BeezaoGA/`, not tracked) is valid (passes toolset/capacity/completeness checks) but
  gives 600 switches for all 12 instances vs 666–745 in the table, so it is not a match.
  Most likely the published AG came from a different GA implementation/configuration.
- The unrelated runs in `output/` (GATeste1, GATestes-2 [same values as GATestes on the rows checked], TCC*, Beezao
  PT small/IPMTC-I, etc.) were not part of these tables.

## Base cases (`base-instances/`) — 2M1, 6M1, 6M2, all with p = 0.5

Source: commit `c4ebd30f` on `master` (these runs were never on the `rework` branch).
Instances are `input/BaseInstances/{2M1,6M1,6M2}/`, `r` = 0.5 / 0.6 / 0.4 respectively.
Validated cell-by-cell against `Resultados SSP-USPrC - Dang reproduzidas.csv`
(1079 cells, 0 mismatches).

| Sheet block | Folder | Runs |
|---|---|---|
| PT | `base-instances/PT-SSP-USPrC/` | 10 (`run-01`…`run-10`), 37 instances each |
| GA | `base-instances/AG-genetic-algorithm/` | 10, 37 instances each |
| Practitioner | `base-instances/Practitioner-single-run/` | 1 run, 37 instances |
| MODELO | `base-instances/Modelo-gurobi-single-run/` | 1 run (`.csv` report, `.sol`, `.txt`) |

`*_analise.csv` next to them are the pre-computed summaries from `master`.

How the sheet columns come from the runs:
- Finished / priority-unfinished / total-unfinished tasks, switch instances, tool switches:
  **mean over the 10 runs** (PT, GA); single value for Practitioner and Modelo.
- PT `Melhor solução S*` = max `Final Solution` over the 10 runs; `Médio da solução S` = mean.
- PT `Desvio Padrão σ` = std-dev of the 10 `Final Solution`s as a **% of their mean**.
- PT `Media das Melhores Soluções iniciais` = mean of `Best Initial`;
  `gap entre S0 e S*` = (S* − S0)/S* × 100.
- PT `Convergência %` = mean(`PTL`) / **500** × 100: these runs used `--PTL 500` (the largest PTL in
  their reports is 499; the sheet's INFO note says 600). `latexTables.py table-pt` defaults
  `--total-ptl` to 600; pass `--total-ptl 500` to reproduce the base-case sheet.
- GA `Resultado` = mean `Final Solution` (identical across runs, deterministic); `Tempo` = mean seconds.
- Modelo `Best Bound` / `Resultado` / `Tempo` come from the `.csv` report; only instances the
  solver finished have one (2M1 up to n=50, 6M1 up to n=50, 6M2 up to n=80).
