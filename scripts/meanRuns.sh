#!/usr/bin/env bash
# Usage: ./scripts/meanRuns.sh [output/GATestes]
# Prints mean of "Final Solution" across all numbered run subdirectories,
# for both MyInstancesSameToolSets and MyInstancesDiferentToolSets.

BASE="${1:-output/GATestes}"

compute_means() {
    local subdir="$1"

    mapfile -t instances < <(
        find "$BASE" -path "*/$subdir/*.csv" -printf '%f\n' \
        | sort -u -V
    )

    for inst in "${instances[@]}"; do
        local sum=0
        local count=0

        for f in "$BASE"/*/"$subdir/$inst"; do
            [[ -f "$f" ]] || continue

            val=$(grep "^Time:" "$f" | awk '{print $NF}')
            [[ -n "$val" ]] || continue

            sum=$(awk "BEGIN { printf \"%.6f\", $sum + $val }")
            (( count++ ))
        done

        if (( count > 0 )); then
            mean=$(awk "BEGIN { printf \"%.2f\", ($sum / $count) / 1000 }")
            echo "$inst | $mean | n=$count"
        fi
    done
}
echo "=== MyInstancesSameToolSets ==="
compute_means "MyInstancesSameToolSets"

echo ""
echo "=== MyInstancesDiferentToolSets ==="
compute_means "MyInstancesDiferentToolSets"
