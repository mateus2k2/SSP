# ./scripts/runMultiple.sh ./output/BasePT1 base PT 999999 tesla
# ./scripts/runMultiple.sh ./output/BaseGA1 base PT 999999 tesla
# ./scripts/runMultiple.sh ./output/BaseModelo1 base modelo 999999 tesla > out 
# ./scripts/runMultiple.sh ./output/BaseHeuristica base modelo 999999 normal


outputFolder=${1:-"./output/Multiplos"}
runMode=${2:-"both"}
type=${3:-"PT"}
head=${4:-"9999"}
env=${5:-"normal"}
licenseFile=${6:-""}

for i in $(seq 1 1)
do
    echo "Running $i"
    ./scripts/runAuto.sh $outputFolder/$i $runMode $type $head $env $licenseFile
done
