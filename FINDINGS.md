# Validation findings — September 2026

Every solution report in `output-final/` (18 416 reports: the runs behind the thesis tables)
was checked against its instance, rule by rule, and every counter and objective recomputed from
the printed schedule. 2 266 reports contained at least one error. This file lists what was found,
what was fixed, and what is still open.

Reproduce with:

```bash
python3 scripts/results/validateRuns.py output-final          # all checks, per folder
python3 scripts/results/buildSpreadsheet.py --compare .tmp     # results vs the Google Sheet export
```

**The practitioner runs in `output-final/` were regenerated on 2026-09-21** with the fixed solver and
now validate with 0 errors; the originals are kept as `*-single-run-pre-fix/` (see §7). The GA, PT and
Modelo runs there still predate the fixes and still show the problems below. Rerunning them leaves
same-toolset and base GA/PT results unchanged (except the three n=212 instances in §3.1);
different-toolset results change.

---

## 1. Wrong values in the reports (errors)

| # | Problem | Where | Status |
|---|---|---|---|
| 1 | `unfineshedPriorityCount` holds the **finished** priority count, and the objective uses it | practitioner: 1560 reports (41 same, 42 diff, 37 base, 1440 Beezao HP) | fixed `f53247b7`; reports regenerated |
| 2 | Op 1 finished without its op 0 | different toolset: PT 322, GA 290, HP 33 reports | fixed `f53247b7` |
| 3 | Op 0 and op 1 of a job on different machines | different toolset: PT 60, HP 41, GA 30, Modelo 3 reports | fixed `f53247b7` |
| 4 | Op 1 not immediately after op 0 (another job in between) | different toolset: HP 40, Modelo 3 reports | fixed `f53247b7` |
| 5 | Operation processed without all its tools | same toolset HP: 2 reports (n=212 v7, v8) | fixed `f53247b7` |

**Causes.** (1) `KTNSReport` returns the finished priority count; `practitioner.cpp` stored it as if it
were the unfinished count. (2)–(3) for GA/PT: `splitSolutionIntoMachines` cuts the operation sequence
by count and could cut between op 0 and op 1, so op 1 opened the next machine and could finish while
op 0 never did (at most one pair per machine boundary). (3)–(4) the practitioner never imposed the
strong chain in different-toolset mode, and the Gurobi model's constraint (9) is only a precedence in
time (weak chain). (5) KTNS loads at most `capacity` tools, so an operation needing more was
"processed" with part of its tools instead of being left unfinished.

**Impact on the published numbers.** Every practitioner "Resultado" and every Beezao HP "FO Dang" is
wrong (e.g. same toolset n=15, p=0.5: the sheet says 11, the schedule is worth 251). In the
different-toolset tables, an op 1 counted without its op 0 inflates the objective by a median 2.1 %
(max 11.5 %) in the 612 affected reports.

## 2. Reporting quirks that do not change feasibility (warnings)

| # | Problem | Where | Status |
|---|---|---|---|
| 6 | Grouped pair printed with op 0 / op 1 durations swapped (total correct) | ~1490 reports (base, same, Modelo, HP) | fixed `f53247b7` |
| 7 | Op 0 charged p0+p1 while op 1 is charged p1 again (wasted time) | all different toolset: 886 reports | fixed `f53247b7` |
| 8 | Gurobi times truncated to int, showing 1-minute "overlaps" | Modelo: 10 reports | fixed `f53247b7` |
| 9 | Tool switch exactly at minute 720, the unsupervised boundary | Modelo: 2 reports | **open, by design** |

(6) `groupJobs` stored `processingTimes` as `[p1, p0]` while both report printers read
`processingTimes[0]` as op 0's time. (7) `groupJobs` added op 1's time to op 0 without grouping the
pair, so op 1's time was spent twice — this made different-toolset schedules finish fewer operations
than they should (e.g. n=1000 p=0.25 practitioner: 455 finished before, 613 after).
(8) `modelo.cpp` passed Gurobi's `double` start/end through `int` parameters, so 1906.9999999 printed
as 1906.

**About #9.** This is a difference of convention, not a bug: the model's constraint (16) allows a
switch when `s mod DAY <= tU` (inclusive), while the decoders allow it only when `s mod DAY < U`
(strict). A switch exactly at minute 720 is therefore feasible for the model and not for the
heuristics. The validator reports it as a warning. To make the model agree exactly, its constraint
would need `h(jk) <= tU - 1`; this has not been changed because it only affects that single minute.

## 3. Instance data

1. **Same toolset n=212 v6, v7, v8**: one operation requires 105 tools while the magazine holds 80,
   so it can never be processed. PT and GA never finish it; the practitioner used to "process" it
   with 80 tools (§1.5). The instances were not modified — the solver now skips such operations.
   As a side effect, GA results on these three instances change (v7: 4015 → 4058, v8: 3848 → 3756).
2. **Same toolset n=75, p=0.24**: a `1z` cell in the priority column. `std::stoi` reads it as 1, and
   the validator reproduces that reading and reports it as a warning.

Check any instance set with `python3 scripts/instances/countTools.py <folder>`.

## 4. Spreadsheet / bookkeeping

1. Seven corrupted cells in the Different Toolset practitioner block (`6,39` instead of 6390,
   `-1,37` instead of -1370, …), from a thousands-separator mishap.
2. `instanceLarge13` and `instanceLarge14` are missing from the "IPMTC II Pratictitioners e GA" tab.
3. The "Convergencia %" column is stored as text (`1.14`, `08.06`) and differs from
   mean(PTL)/limit × 100 by up to 0.01.
4. The "Dang reproduzidas" INFO note says `--PTL 600`, but those runs used 500 (the largest PTL in
   their reports is 499). The convergence column matches 500.
5. The GA side table in the Beezao tab (931: 745, 932: 684, …) matches no run in the repository. A
   fresh GA run is valid but gives 600 switches for all 12 instances, so the published values most
   likely came from a different GA implementation or configuration.

Everything else reproduces: `buildSpreadsheet.py --values reported --compare .tmp` matches about
27 500 cells of the sheet, with only the discrepancies above.

## 5. Other code problems found while investigating

| Problem | Status |
|---|---|
| Practitioner balancing looped forever when a machine stayed empty (BaseInstances 6M1/6M2 n=5 hung) | fixed `f53247b7` |
| Gurobi model's output paths were built as `"./" + folder`, so absolute paths failed (error 10013) and no report was written | fixed `c1d1ab38` |
| A failed model run returned 0 and left an empty report, so batch runs looked successful | fixed `c1d1ab38` |
| The binary needed `LD_LIBRARY_PATH` to find `libgurobi`; `GUROBI_HOME`/`GRB_LICENSE_FILE` had no defaults | fixed `c1d1ab38` (rpath + Makefile defaults) |
| `--TIME_LIMIT` is in **minutes** but `--help` said seconds (`runExperiment.sh` passes 120, i.e. 2 h) | doc fixed `c1d1ab38` |
| Practitioner thresholds B1/B2 were fractions of the horizon **in days** compared against workloads in minutes, so balancing never ran | fixed `01b7f547` |
| The model read `tU` as the unsupervised shift's length, the decoders as its start minute | fixed `01b7f547` |
| Beezao loader set `unsupervised = horizon` (wrong unit; accidentally disabled the unsupervised logic) | fixed `01b7f547`, now `DAY` = "no unsupervised period" |
| **The Makefile had no header dependencies**, so editing a header recompiled nothing and objects were silently built against different versions of a class | fixed `01b7f547` (`-MMD -MP`) |

The last one is worth remembering: any build made after editing a header, before this fix, could have
been inconsistent without any warning. Use `make clean` when in doubt about older results.

## 6. Verification

Old and new binaries were built clean and run over the same inputs:

| Check | Result |
|---|---|
| Practitioner, all 1561 instances (same, diff, base, Beezao) | 0 validation errors; counters unchanged except the 2 instances where balancing now runs and the 42 different-toolset ones |
| GA, 10 sampled instances | identical to the previous commit; same-toolset values match the published ones |
| PT, short runs | 0 validation errors |
| Gurobi model, same and different toolset n=15 | 0 validation errors; strong chain holds, times no longer truncated |
| Existing `output-final` reports | still parse and validate with the same findings as before |

## 7. Regenerated runs and what is still pending

The practitioner was rerun over all four instance sets on 2026-09-21 (`01b7f547`): 1561 reports,
0 validation errors. The originals are kept beside them as `*-single-run-pre-fix/`. Against those:

| Set | Objective changed | Switches | Finished | Why |
|---|---|---|---|---|
| same toolset | 41/42 | 3 | 2 | priority counter; n=212 v7/v8 over-capacity op; n=15 p=0.75 balancing |
| different toolset | 42/42 | 41 | 33 | priority counter, strong chain, op 1's time no longer spent twice |
| base | 37/37 | 0 | 0 | priority counter only |
| Beezao HP | 1440/1440 | 1173 | 1191 | priority counter **and** the `getMakespan()` fix (`ecd94128`): the horizon came from the wrong CSV column and was about ten times too small, so jobs were left unfinished. All 200 jobs now finish (instance 931: 909 -> 927 switches) |

The spreadsheet was rebuilt from them: `Resultados-SSP-USPrC.xlsx` now needs **no** corrections
(reported and recomputed values agree), and validation errors across `output-final/` dropped from
2266 to 706 reports.

Note for the thesis tables: the HP column changes everywhere, and for the Beezao instances it changes
for a second reason (the horizon fix), not only the objective bug. The Beezao PT runs were produced by
an even older version whose header says horizon = 1 day, unsupervised = 1068; rerunning them would
move those numbers too.

## 8. Open items

1. **Rerun the remaining experiments.** The different-toolset GA (320 reports with errors), PT (382)
   and Modelo (4) runs in `output-final/` still come from the buggy code. GA/PT runs take hours; the
   model needs Gurobi.
2. **Practitioner B1/B2.** They are now fractions of the horizon in minutes, which is the consistent
   reading, but Holanda et al.'s definition was not available to confirm it.
3. **Beezao PT runs.** They still carry the old horizon (1 day) and the spurious 1068-minute
   unsupervised cutoff described in §9, so in the IPMTC table PT and HP now rest on different
   assumptions. Rerunning the 12 table instances takes about an hour; the full tab is a multi-day job.
   Not rerun yet, by decision.
4. **`Time` in report footers** is milliseconds for GA/PT/practitioner and seconds for the Gurobi
   model. Documented in `scripts/ssp/reports.py` rather than changed, because changing it would
   invalidate every existing report and the spreadsheet's time columns.
5. **Minute-720 switch in the model** (§2.9).

## 9. The Beezao (IPMTC) horizon is delta*, from the paper

The `.PMTC` files carry no horizon and no unsupervised period. Dang et al. (2023), Appendix D, state
how they ran these instances: no unsupervised hours (rho_U = 0), every job priority (rho_P = 1),
r = 0, c_f = 0, c_v = 1, c_p = 30, and **H = delta\***, "the makespan obtained by Beezao et al.
excluding the idle time caused by tool switches". Those delta\* values are printed in their Table D.1.

`input/BeezaoRaw/alns-original.csv` holds ten ALNS runs per instance, and the mean of its
**`Timeofjobsprocessing`** column reproduces Table D.1's delta\* **exactly for all 12 instances**
(instance 931: 1068.80). That is the column `getMakespan()` originally read, and the horizon binds:
delta\* is about 1068 minutes against about 1072 minutes of work per machine, so a few operations do
not fit.

This was broken and is now restored:

| | horizon for instance 931 | HP result |
|---|---|---|
| original code (what produced the published runs) | 1068 = delta\* | 196 of 200 finished, 909 switches |
| after `ecd94128` "fix" (read `makespan`, x2) | 10898 | 200 finished, 927 switches |
| now (`getMakespan()` = mean `Timeofjobsprocessing`, no x2) | 1068 = delta\* | 196 finished, 909 switches |

`ecd94128` changed the column to `makespan`, which *includes* the tool switching time
(5449 = 1068.8 + 4380.6), and kept an extra factor of two, so the horizon stopped binding and every
job finished. Reading delta\* again reproduces the published HP numbers exactly. The report header
now carries the horizon in minutes (§ above), which this needs: delta\* is less than one day.

The Beezao PT runs in `output-final/` still carry the old header (horizon 1 day, unsupervised 1068)
and have not been rerun.

## 10. Why the GA returns the same switch count for every IPMTC-II instance

Running the GA over the 12 instances of the Beezao table with a fixed seed gives **exactly** the same
number of switches on all of them (600 with seed 42, 595 with seed 7, 624 with seed 2024). That is not
a coincidence and not a bug: for our objective those 12 files are the same problem.

| What the files contain | 931 | 932 | 933 | 946-948 | 952-954 | 958-960 |
|---|---|---|---|---|---|---|
| tool matrix (tool set of every job) | identical across all twelve | = | = | = | = | = |
| processing times | A | A | A | B | C | D |
| switching time (line 2 of the .PMTC) | 42 | 68 | 1 | ... | ... | ... |

Within a group of three (931/932/933) the files differ **only** in the switching time; between groups
they also differ in the processing times. Our model has neither: the SSP-USPrC objective counts tool
switches (it has no switching *time*), and with the Beezao cost parameters
(`--PROFITYFINISHED 0 --COSTSWITCHINSTANCE 0`) the objective is exactly `-switches`. Processing times
could only enter through the horizon or the unsupervised period, and neither binds here (§9), while
the GA splits the sequence across machines by operation *count*, not by time. So the GA is solving one
and the same instance twelve times, and the seed is the only thing that changes.

The practitioner does use processing times, in its workload balancing, and it behaves accordingly:
927 switches for 931-933, 889 for 946-948, 933 for 952-954, 923 for 958-960 — constant inside each
group of equal processing times, different between groups.

Across the whole IPMTC-II set the redundancy is the same: 1440 files, **48 distinct tool matrices**,
each shared by 30 files.

This also explains why the published AG column (745, 684, 718, ...) cannot be reproduced here: it
varies from instance to instance within a group, so it must come from a model in which the switching
time matters — which is Beezao's original IPMTC objective, not the SSP-USPrC one. Making our solver
distinguish these instances would require modelling switching time (or splitting machines by time,
which `splitSolutionIntoMachinesByTime()` does but is not used).
