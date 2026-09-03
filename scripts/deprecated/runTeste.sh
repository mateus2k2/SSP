# DEPRECATED (2026-09-03): exact subset of ../runPrint.sh (same "print line 3
# of every .dat file" loop, minus the every-3rd-file blank line and the
# folder argument), with a hardcoded path from another machine. Use
# runPrint.sh instead.
for file in /home/mateus/WSL/IC/SSP/input/MyInstancesSameToolSets/*.dat; do
  if [ -f "$file" ]; then
    sed -n '3p' "$file"
  fi
done