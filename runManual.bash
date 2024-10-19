#------------------------------------------------------------
# SAME TOOL SET
#------------------------------------------------------------

echo "RODANDO INSTANCIAS DE SAME TOOL SET"
echo 'n=75,p=0.24,r=0.5,t=112,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=75,p=0.24,r=0.5,t=650,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=75,p=0.24,r=0.5,t=650,v0.csv"         --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 2 --UNSUPERVISED_MINUTS 720
echo 'n=75,p=0.51,r=0.5,t=112,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=75,p=0.51,r=0.5,t=650,v1.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=75,p=0.51,r=0.5,t=650,v1.csv"         --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 2 --UNSUPERVISED_MINUTS 720
echo 'n=75,p=0.75,r=0.5,t=112,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=75,p=0.75,r=0.5,t=650,v2.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=75,p=0.75,r=0.5,t=650,v2.csv"         --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 2 --UNSUPERVISED_MINUTS 720
echo 'n=212,p=0.25,r=0.4,t=296,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=212,p=0.25,r=0.4,t=1390,v6.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=212,p=0.25,r=0.4,t=1390,v6.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 6 --UNSUPERVISED_MINUTS 720
echo 'n=212,p=0.50,r=0.4,t=296,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=212,p=0.50,r=0.4,t=1390,v7.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=212,p=0.50,r=0.4,t=1390,v7.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 6 --UNSUPERVISED_MINUTS 720
echo 'n=212,p=0.75,r=0.4,t=296,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=212,p=0.75,r=0.4,t=1390,v8.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=212,p=0.75,r=0.4,t=1390,v8.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 6 --UNSUPERVISED_MINUTS 720
echo 'n=228,p=0.25,r=0.6,t=364,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=228,p=0.25,r=0.6,t=1520,v3.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=228,p=0.25,r=0.6,t=1520,v3.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 6 --UNSUPERVISED_MINUTS 720
echo 'n=228,p=0.50,r=0.6,t=364,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=228,p=0.50,r=0.6,t=1520,v4.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=228,p=0.50,r=0.6,t=1520,v4.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 6 --UNSUPERVISED_MINUTS 720
echo 'n=228,p=0.75,r=0.6,t=364,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=228,p=0.75,r=0.6,t=1520,v5.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=228,p=0.75,r=0.6,t=1520,v5.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 6 --UNSUPERVISED_MINUTS 720
echo 'n=399,p=0.25,r=0.5,t=598,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=399,p=0.25,r=0.5,t=2310,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=399,p=0.25,r=0.5,t=2310,v0.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 10 --UNSUPERVISED_MINUTS 720
echo 'n=399,p=0.50,r=0.5,t=598,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=399,p=0.50,r=0.5,t=2310,v1.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=399,p=0.50,r=0.5,t=2310,v1.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 10 --UNSUPERVISED_MINUTS 720
echo 'n=399,p=0.75,r=0.5,t=598,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=399,p=0.75,r=0.5,t=2310,v2.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=399,p=0.75,r=0.5,t=2310,v2.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 10 --UNSUPERVISED_MINUTS 720
echo 'n=499,p=0.25,r=0.5,t=748,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=499,p=0.25,r=0.5,t=2690,v3.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=499,p=0.25,r=0.5,t=2690,v3.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 4 --UNSUPERVISED_MINUTS 720
echo 'n=499,p=0.50,r=0.5,t=748,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=499,p=0.50,r=0.5,t=2690,v4.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=499,p=0.50,r=0.5,t=2690,v4.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 4 --UNSUPERVISED_MINUTS 720
echo 'n=499,p=0.75,r=0.5,t=748,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=499,p=0.75,r=0.5,t=2690,v5.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=499,p=0.75,r=0.5,t=2690,v5.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 4 --UNSUPERVISED_MINUTS 720
echo 'n=600,p=0.25,r=0.5,t=900,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=600,p=0.25,r=0.5,t=3096,v6.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=600,p=0.25,r=0.5,t=3096,v6.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 5 --UNSUPERVISED_MINUTS 720
echo 'n=600,p=0.50,r=0.5,t=900,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=600,p=0.50,r=0.5,t=3096,v7.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=600,p=0.50,r=0.5,t=3096,v7.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 5 --UNSUPERVISED_MINUTS 720
echo 'n=600,p=0.75,r=0.5,t=900,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=600,p=0.75,r=0.5,t=3096,v8.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=600,p=0.75,r=0.5,t=3096,v8.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 5 --UNSUPERVISED_MINUTS 720
echo 'n=699,p=0.25,r=0.5,t=1048,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=699,p=0.25,r=0.5,t=3420,v9.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=699,p=0.25,r=0.5,t=3420,v9.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 6 --UNSUPERVISED_MINUTS 720
echo 'n=699,p=0.50,r=0.5,t=1048,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=699,p=0.50,r=0.5,t=3420,v10.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=699,p=0.50,r=0.5,t=3420,v10.csv"   --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 6 --UNSUPERVISED_MINUTS 720
echo 'n=699,p=0.75,r=0.5,t=1048,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=699,p=0.75,r=0.5,t=3420,v11.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=699,p=0.75,r=0.5,t=3420,v11.csv"   --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 6 --UNSUPERVISED_MINUTS 720
echo 'n=800,p=0.25,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=800,p=0.25,r=0.6,t=3527,v12.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=800,p=0.25,r=0.6,t=3527,v12.csv"   --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 7 --UNSUPERVISED_MINUTS 720
echo 'n=800,p=0.50,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=800,p=0.50,r=0.6,t=3527,v13.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=800,p=0.50,r=0.6,t=3527,v13.csv"   --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 7 --UNSUPERVISED_MINUTS 720
echo 'n=800,p=0.75,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=800,p=0.75,r=0.6,t=3527,v14.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=800,p=0.75,r=0.6,t=3527,v14.csv"   --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 7 --UNSUPERVISED_MINUTS 720
echo 'n=899,p=0.25,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=899,p=0.25,r=0.6,t=3746,v15.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=899,p=0.25,r=0.6,t=3746,v15.csv"   --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 8 --UNSUPERVISED_MINUTS 720
echo 'n=899,p=0.50,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=899,p=0.50,r=0.6,t=3746,v16.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=899,p=0.50,r=0.6,t=3746,v16.csv"   --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 8 --UNSUPERVISED_MINUTS 720
echo 'n=899,p=0.75,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=899,p=0.75,r=0.6,t=3746,v17.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=899,p=0.75,r=0.6,t=3746,v17.csv"   --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 8 --UNSUPERVISED_MINUTS 720
echo 'n=1000,p=0.25,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=1000,p=0.25,r=0.6,t=3950,v18.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=1000,p=0.25,r=0.6,t=3950,v18.csv" --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 9 --UNSUPERVISED_MINUTS 720
echo 'n=1000,p=0.50,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=1000,p=0.50,r=0.6,t=3950,v19.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=1000,p=0.50,r=0.6,t=3950,v19.csv" --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 9 --UNSUPERVISED_MINUTS 720
echo 'n=1000,p=0.75,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=1000,p=0.75,r=0.6,t=3950,v20.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=1000,p=0.75,r=0.6,t=3950,v20.csv" --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 9 --UNSUPERVISED_MINUTS 720
echo 'n=1236,p=0.25,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=1236,p=0.25,r=0.6,t=4431,v21.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=1236,p=0.25,r=0.6,t=4431,v21.csv" --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 11 --UNSUPERVISED_MINUTS 720
echo 'n=1236,p=0.50,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=1236,p=0.50,r=0.6,t=4431,v22.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=1236,p=0.50,r=0.6,t=4431,v22.csv" --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 11 --UNSUPERVISED_MINUTS 720
echo 'n=1236,p=0.75,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesSameToolSets/n=1236,p=0.75,r=0.6,t=4431,v23.csv" "./input/ToolSetInt.csv" "./output/MyInstancesSameToolSets/n=1236,p=0.75,r=0.6,t=4431,v23.csv" --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 11 --UNSUPERVISED_MINUTS 720

#------------------------------------------------------------
# DIFERENT TOOL SET
#------------------------------------------------------------

echo "RODANDO INSTANCIAS DE DIFERENT TOOL SET"
echo 'n=75,p=0.24,r=0.5,t=112,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=75,p=0.24,r=0.5,t=112,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=75,p=0.24,r=0.5,t=112,v0.csv"       --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 2 --UNSUPERVISED_MINUTS 720
echo 'n=75,p=0.51,r=0.5,t=112,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=75,p=0.51,r=0.5,t=112,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=75,p=0.51,r=0.5,t=112,v0.csv"       --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 2 --UNSUPERVISED_MINUTS 720
echo 'n=75,p=0.75,r=0.5,t=112,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=75,p=0.75,r=0.5,t=112,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=75,p=0.75,r=0.5,t=112,v0.csv"       --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 2 --UNSUPERVISED_MINUTS 720
echo 'n=212,p=0.25,r=0.4,t=296,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=212,p=0.25,r=0.4,t=296,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=212,p=0.25,r=0.4,t=296,v0.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 6 --UNSUPERVISED_MINUTS 720
echo 'n=212,p=0.50,r=0.4,t=296,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=212,p=0.50,r=0.4,t=296,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=212,p=0.50,r=0.4,t=296,v0.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 6 --UNSUPERVISED_MINUTS 720
echo 'n=212,p=0.75,r=0.4,t=296,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=212,p=0.75,r=0.4,t=296,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=212,p=0.75,r=0.4,t=296,v0.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 6 --UNSUPERVISED_MINUTS 720
echo 'n=228,p=0.25,r=0.6,t=364,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=228,p=0.25,r=0.6,t=364,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=228,p=0.25,r=0.6,t=364,v0.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 6 --UNSUPERVISED_MINUTS 720
echo 'n=228,p=0.50,r=0.6,t=364,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=228,p=0.50,r=0.6,t=364,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=228,p=0.50,r=0.6,t=364,v0.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 6 --UNSUPERVISED_MINUTS 720
echo 'n=228,p=0.75,r=0.6,t=364,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=228,p=0.75,r=0.6,t=364,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=228,p=0.75,r=0.6,t=364,v0.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 6 --UNSUPERVISED_MINUTS 720
echo 'n=399,p=0.25,r=0.5,t=598,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=399,p=0.25,r=0.5,t=598,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=399,p=0.25,r=0.5,t=598,v0.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 10 --UNSUPERVISED_MINUTS 720
echo 'n=399,p=0.50,r=0.5,t=598,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=399,p=0.50,r=0.5,t=598,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=399,p=0.50,r=0.5,t=598,v0.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 11 --UNSUPERVISED_MINUTS 720
echo 'n=399,p=0.75,r=0.5,t=598,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=399,p=0.75,r=0.5,t=598,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=399,p=0.75,r=0.5,t=598,v0.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 2 --DAYS 11 --UNSUPERVISED_MINUTS 720
echo 'n=499,p=0.25,r=0.5,t=748,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=499,p=0.25,r=0.5,t=748,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=499,p=0.25,r=0.5,t=748,v0.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 4 --UNSUPERVISED_MINUTS 720
echo 'n=499,p=0.50,r=0.5,t=748,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=499,p=0.50,r=0.5,t=748,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=499,p=0.50,r=0.5,t=748,v0.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 4 --UNSUPERVISED_MINUTS 720
echo 'n=499,p=0.75,r=0.5,t=748,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=499,p=0.75,r=0.5,t=748,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=499,p=0.75,r=0.5,t=748,v0.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 4 --UNSUPERVISED_MINUTS 720
echo 'n=600,p=0.25,r=0.5,t=900,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=600,p=0.25,r=0.5,t=900,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=600,p=0.25,r=0.5,t=900,v0.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 5 --UNSUPERVISED_MINUTS 720
echo 'n=600,p=0.50,r=0.5,t=900,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=600,p=0.50,r=0.5,t=900,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=600,p=0.50,r=0.5,t=900,v0.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 5 --UNSUPERVISED_MINUTS 720
echo 'n=600,p=0.75,r=0.5,t=900,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=600,p=0.75,r=0.5,t=900,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=600,p=0.75,r=0.5,t=900,v0.csv"     --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 5 --UNSUPERVISED_MINUTS 720
echo 'n=699,p=0.25,r=0.5,t=1048,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=699,p=0.25,r=0.5,t=1048,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=699,p=0.25,r=0.5,t=1048,v0.csv"   --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 6 --UNSUPERVISED_MINUTS 720
echo 'n=699,p=0.50,r=0.5,t=1048,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=699,p=0.50,r=0.5,t=1048,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=699,p=0.50,r=0.5,t=1048,v0.csv"   --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 6 --UNSUPERVISED_MINUTS 720
echo 'n=699,p=0.75,r=0.5,t=1048,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=699,p=0.75,r=0.5,t=1048,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=699,p=0.75,r=0.5,t=1048,v0.csv"   --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 6 --UNSUPERVISED_MINUTS 720
echo 'n=800,p=0.25,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=800,p=0.25,r=0.6,t=1265,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=800,p=0.25,r=0.6,t=1265,v0.csv"   --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 7 --UNSUPERVISED_MINUTS 720
echo 'n=800,p=0.50,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=800,p=0.50,r=0.6,t=1265,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=800,p=0.50,r=0.6,t=1265,v0.csv"   --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 7 --UNSUPERVISED_MINUTS 720
echo 'n=800,p=0.75,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=800,p=0.75,r=0.6,t=1265,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=800,p=0.75,r=0.6,t=1265,v0.csv"   --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 7 --UNSUPERVISED_MINUTS 720
echo 'n=899,p=0.25,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=899,p=0.25,r=0.6,t=1265,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=899,p=0.25,r=0.6,t=1265,v0.csv"   --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 8 --UNSUPERVISED_MINUTS 720
echo 'n=899,p=0.50,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=899,p=0.50,r=0.6,t=1265,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=899,p=0.50,r=0.6,t=1265,v0.csv"   --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 8 --UNSUPERVISED_MINUTS 720
echo 'n=899,p=0.75,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=899,p=0.75,r=0.6,t=1265,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=899,p=0.75,r=0.6,t=1265,v0.csv"   --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 8 --UNSUPERVISED_MINUTS 720
echo 'n=1000,p=0.25,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=1000,p=0.25,r=0.6,t=1265,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=1000,p=0.25,r=0.6,t=1265,v0.csv" --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 9 --UNSUPERVISED_MINUTS 720
echo 'n=1000,p=0.50,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=1000,p=0.50,r=0.6,t=1265,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=1000,p=0.50,r=0.6,t=1265,v0.csv" --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 9 --UNSUPERVISED_MINUTS 720
echo 'n=1000,p=0.75,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=1000,p=0.75,r=0.6,t=1265,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=1000,p=0.75,r=0.6,t=1265,v0.csv" --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 9 --UNSUPERVISED_MINUTS 720
echo 'n=1236,p=0.25,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=1236,p=0.25,r=0.6,t=1265,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=1236,p=0.25,r=0.6,t=1265,v0.csv" --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 11 --UNSUPERVISED_MINUTS 720
echo 'n=1236,p=0.50,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=1236,p=0.50,r=0.6,t=1265,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=1236,p=0.50,r=0.6,t=1265,v0.csv" --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 11 --UNSUPERVISED_MINUTS 720
echo 'n=1236,p=0.75,r=0.6,t=1265,v0.csv'
./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=1236,p=0.75,r=0.6,t=1265,v0.csv" "./input/ToolSetInt.csv" "./output/MyInstancesDiferentToolSets/n=1236,p=0.75,r=0.6,t=1265,v0.csv" --TEMP_INIT 0.1 --TEMP_FIM 0.5 --N_REPLICAS 11 --MCL 400 --PTL 75000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 20000 --CAPACITY 80 --MACHINES 6 --DAYS 11 --UNSUPERVISED_MINUTS 720

