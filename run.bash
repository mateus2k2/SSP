#------------------------------------------------------------
# MyInstancesDiferentToolSets
#------------------------------------------------------------

instancesFolder=./input/MyInstancesDiferentToolSets
toolSetsFile=./input/ToolSetInt.csv

outputFolder=./output/MyInstancesDiferentToolSets2M
echo "RODANDO INSTANCIAS DE $instancesFolder COM 2 MAQUINAS"
for entry in $(ls -v "$instancesFolder"/*)
do
    echo "$entry"
    filename=$(basename "$entry")
    ./src/out/mainCpp "$instancesFolder/$filename" "$toolSetsFile" "$outputFolder/$filename" --TEMP_INIT 0.2 --TEMP_FIM 1 --N_REPLICAS 16 --MCL 400 --PTL 1000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 35000 --CAPACITY 80 --MACHINES 2 --DAYS 7 --UNSUPERVISED_MINUTS 720
done


outputFolder=./output/MyInstancesDiferentToolSets6M
echo "RODANDO INSTANCIAS DE $instancesFolder COM 6 MAQUINAS"
for entry in $(ls -v "$instancesFolder"/*)
do
    echo "$entry"
    filename=$(basename "$entry")
    ./src/out/mainCpp "$instancesFolder/$filename" "$toolSetsFile" "$outputFolder/$filename" --TEMP_INIT 0.2 --TEMP_FIM 1 --N_REPLICAS 16 --MCL 400 --PTL 1000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 35000 --CAPACITY 80 --MACHINES 6 --DAYS 7 --UNSUPERVISED_MINUTS 720
done

#------------------------------------------------------------
# MyInstancesSameToolSets
#------------------------------------------------------------

instancesFolder=./input/MyInstancesSameToolSets
toolSetsFile=./input/ToolSetInt.csv

outputFolder=./output/MyInstancesSameToolSets2M
echo "RODANDO INSTANCIAS DE $instancesFolder COM 2 MAQUINAS"
for entry in $(ls -v "$instancesFolder"/*)
do
    echo "$entry"
    filename=$(basename "$entry")
    ./src/out/mainCpp "$instancesFolder/$filename" "$toolSetsFile" "$outputFolder/$filename" --TEMP_INIT 0.2 --TEMP_FIM 1 --N_REPLICAS 16 --MCL 400 --PTL 1000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 35000 --CAPACITY 80 --MACHINES 2 --DAYS 7 --UNSUPERVISED_MINUTS 720
done

outputFolder=./output/MyInstancesSameToolSets6M
echo "RODANDO INSTANCIAS DE $instancesFolder COM 6 MAQUINAS"
for entry in $(ls -v "$instancesFolder"/*)
do
    echo "$entry"
    filename=$(basename "$entry")
    ./src/out/mainCpp "$instancesFolder/$filename" "$toolSetsFile" "$outputFolder/$filename" --TEMP_INIT 0.2 --TEMP_FIM 1 --N_REPLICAS 16 --MCL 400 --PTL 1000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 35000 --CAPACITY 80 --MACHINES 6 --DAYS 7 --UNSUPERVISED_MINUTS 720
done