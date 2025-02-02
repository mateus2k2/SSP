outputFolder=${1:-"./output/SameToolSet10"}
runMode=${2:-"both"}

for i in $(seq 1 10)
do
    echo "Running $i"
    ./scripts/runAuto.sh $outputFolder/$i diferent > out
done
