# ./scripts/runMultiple.sh ./output/SBPOPequenas same PT 9

outputFolder=${1:-"./output/Multiplos"}
runMode=${2:-"both"}
type=${3:-"PT"} # modelo practitioner
head=${4:-"9999"} # quantas instâncias rodar

for i in $(seq 1 10)
do
    echo "Running $i"
    ./scripts/runAuto.sh $outputFolder/$i $runMode
done
