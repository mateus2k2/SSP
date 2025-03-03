for file in /home/mateus/WSL/IC/SSP/input/MyInstancesSameToolSets/*.dat; do
  if [ -f "$file" ]; then
    sed -n '3p' "$file"
  fi
done