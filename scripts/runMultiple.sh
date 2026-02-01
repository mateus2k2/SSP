# ./scripts/runMultiple.sh ./output/SBPOPequenas same PT 9

outputFolder=${1:-"./output/Multiplos"}
runMode=${2:-"both"}
type=${3:-"PT"}
head=${4:-"9999"}
env=${5:-"normal"}
licenseFile=${6:-""}

for i in $(seq 1 10)
do
    echo "Running $i"
    ./scripts/runAuto.sh $outputFolder/$i $runMode $type $head $env $licenseFile
done
