outputFolder=${1:-"./output/Exemplo"}
runMode=${2:-"both"}
compileVar=${3:-"1"}

[ ! -d "$outputFolder" ] && mkdir -p "$outputFolder"
[ ! -d "$outputFolder/MyInstancesSameToolSets" ] && mkdir -p "$outputFolder/MyInstancesSameToolSets"
[ ! -d "$outputFolder/MyInstancesDiferentToolSets" ] && mkdir -p "$outputFolder/MyInstancesDiferentToolSets"

make compilePT DEBUG_MODE=0 GATILHO_MODE=0 FAST_MODE=0

run_instances() {
    local instancesFolder=$1
    local outputFolder=$2
    local instanceMode=$3

    echo "RODANDO INSTANCIAS DE $instancesFolder"
    local counter=1
    for entry in $(ls -v "$instancesFolder"/*.csv)
    do
        filename=$(basename "$entry")
        local timestamp=$(TZ="America/Sao_Paulo" date "+%Y-%m-%d %H:%M:%S.%3N")
        echo "$timestamp - $counter ./src/out/mainCpp $instancesFolder/$filename $toolSetsFile $outputFolder/$filename"
        ./src/out/mainCpp "$instancesFolder/$filename" "$toolSetsFile" "$outputFolder/$filename" \
            --TEMP_INIT 0.1 \
            --TEMP_FIM 0.5 \
            --N_REPLICAS 11 \
            --MCL 400 \
            --PTL 100 \
            --TEMP_DIST 1 \
            --TYPE_UPDATE 1 \
            --INIT_SOL_TYPE 1 \
            --TEMP_UPDATE 5 \
            --PTL_TEMP_UPDATE_PROPORTION 1 \
            --DIFERENT_TOOLSETS_MODE $instanceMode
        counter=$((counter+1))

        # stringToSend="$counter - Instância $instancesFolder/$filename finalizada"
        # wget --header="Content-Type: application/json" \
        #      --post-data='{"chat_id": "336418081", "text": "'"$stringToSend"'"}' \
        #      "https://api.telegram.org/bot8094164826:AAF_tYz1mWAB-site3dt1iFJUMPeAQxH148/sendMessage" \
        #      -O - > /dev/null
    done

}

toolSetsFile=./input/Processed/ToolSetInt.csv

if [ "$runMode" = "both" ] 
then
    instancesFolder=./input/MyInstancesSameToolSets
    run_instances "$instancesFolder" "$outputFolder/MyInstancesSameToolSets" 0

    instancesFolder=./input/MyInstancesDiferentToolSets
    run_instances "$instancesFolder" "$outputFolder/MyInstancesDiferentToolSets" 1

elif [ "$runMode" = "same" ] 
then
    instancesFolder=./input/MyInstancesSameToolSets
    run_instances "$instancesFolder" "$outputFolder/MyInstancesSameToolSets" 0

elif [ "$runMode" = "diferent" ] 
then
    instancesFolder=./input/MyInstancesDiferentToolSets
    run_instances "$instancesFolder" "$outputFolder/MyInstancesDiferentToolSets" 1
else
    echo "Modo de execução inválido"
    exit 1
fi


