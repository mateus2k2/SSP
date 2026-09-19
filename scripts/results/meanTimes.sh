#!/usr/bin/env bash
# Mean run time (report "Time:", ms -> s) per instance across repeated runs.
#
# Usage: ./scripts/results/meanTimes.sh <folder>
#
# Every report under <folder> is grouped by file name, so run-01/x.csv,
# run-02/x.csv, ... (output-final/ layout) or 1/MyInstancesSameToolSets/x.csv,
# 2/MyInstancesSameToolSets/x.csv, ... (runRepeated.sh layout) average into one
# line. A non-run parent folder (MyInstancesSameToolSets, ...) is kept in the key.
# GA, PT and practitioner report Time in ms; the Gurobi model reports seconds,
# so its values come out divided by 1000.

set -euo pipefail

base="${1:?usage: meanTimes.sh <folder>}"

find "$base" -type f \( -name '*.csv' -o -name '*.txt' -o -name '*.PMTC' \) -print0 \
    | xargs -0 grep -H '^Time:' \
    | awk -F: '{
        n = split($1, parts, "/")
        key = parts[n]
        if (n > 1 && parts[n-1] !~ /^(run-)?[0-9]+$/) key = parts[n-1] "/" key
        sum[key] += $NF; count[key]++
      }
      END { for (k in sum) printf "%s | %.2f | n=%d\n", k, sum[k] / count[k] / 1000, count[k] }' \
    | sort -V
