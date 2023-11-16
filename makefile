loadDataCpp:
	clear && g++ src/MeuCpp/loadData.cpp -o src/out/loadData -Wall && clear && src/out/loadData

mainCpp:
	clear && g++ src/MeuCpp/main.cpp -o src/out/main -Wall && clear && src/out/main

mainPy:
	clear src/MeuPy/main.py

git:
	clear && git add . && git commit -m "update" && git push origin master