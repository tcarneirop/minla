g++ -std=c++11 -O3 src/grafo.cpp main.cpp -o minla

g++ -std=c++11 -O3 src/grafo.cpp src/heuristic.cpp  src/hillclimb.cpp src/localsearch.cpp src/full_perm.cpp main.cpp -o minla
