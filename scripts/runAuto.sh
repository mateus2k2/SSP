outputFolder=./output/Exemplo

[ ! -d "$outputFolder" ] && mkdir -p "$outputFolder"
[ ! -d "$outputFolder/MyInstancesSameToolSets" ] && mkdir -p "$outputFolder/MyInstancesSameToolSets"
[ ! -d "$outputFolder/MyInstancesDiferentToolSets" ] && mkdir -p "$outputFolder/MyInstancesDiferentToolSets"

make CompilePT DEBUGMODE=0

toolSetsFile=./input/Processed/ToolSetInt.csv

#------------------------------------------------------------
# MyInstancesSameToolSets
#------------------------------------------------------------

instancesFolder=./input/MyInstancesSameToolSets

echo "RODANDO INSTANCIAS DE $instancesFolder"
for entry in $(ls -v "$instancesFolder"/*.csv)
do
    echo "./src/out/mainCpp "$instancesFolder/$filename" "$toolSetsFile" "$outputFolder/MyInstancesSameToolSets/$filename""
    filename=$(basename "$entry")
    ./src/out/mainCpp "$instancesFolder/$filename" "$toolSetsFile" "$outputFolder/MyInstancesSameToolSets/$filename" \
        --TEMP_INIT 0.2 \
        --TEMP_FIM 1 \
        --N_REPLICAS 16 \
        --MCL 400 \
        --PTL 100 \
        --TEMP_DIST 1 \
        --TYPE_UPDATE 2 \
        --TEMP_UPDATE 35000 \
        --INIT_SOL_TYPE 1 \
        --RESULT_REPORT 1 \
        --INSTANCE_MODE 1
done

#------------------------------------------------------------
# MyInstancesDiferentToolSets
#------------------------------------------------------------

instancesFolder=./input/MyInstancesDiferentToolSets

outputFolder=./output/Ajuste2-PTL100/MyInstancesDiferentToolSets
echo "RODANDO INSTANCIAS DE $instancesFolder"
for entry in $(ls -v "$instancesFolder"/*.csv)
do
    echo "$entry"
    filename=$(basename "$entry")
    ./src/out/mainCpp "$instancesFolder/$filename" "$toolSetsFile" "$outputFolder/MyInstancesDiferentToolSets/$filename" \
        --TEMP_INIT 0.2  \
        --TEMP_FIM 1  \
        --N_REPLICAS 16  \
        --MCL 400  \
        --PTL 100  \
        --TEMP_DIST 1  \
        --TYPE_UPDATE 2  \
        --TEMP_UPDATE 35000 \
        --INIT_SOL_TYPE 1 \
        --RESULT_REPORT 1 \
        --INSTANCE_MODE 0
done