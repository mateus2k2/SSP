instancesFolder=./input/MyInstancesDiferentToolSets
outputFolder=./output

toolSetsFile=./input/ToolSetInt.csv

for entry in "$instancesFolder"/*
do
    filename=$(basename "$entry")
    ./src/out/mainCpp $instancesFolder/$filename $toolSetsFile $outputFolder/$filename --TEMP_INIT 0.2 --TEMP_FIM 1 --N_REPLICAS 16 --MCL 400 --PTL 1000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 35000
done
