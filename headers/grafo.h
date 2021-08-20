#ifndef GRAFO_H
#define GRAFO_H

#include <map>
#include <set>
#include "def.h"
#include <cstring>
#include <stdlib.h>
#include <vector>
#include <set>
#include <cmath>
#include <string>
#include "functional"
#include "solution.h"

#define MAX 10000
#define ITER 100
#define TAM 1000

//using namespace std;

using lsmethod = function<int( int[], int, bool)>;

class Grafo
{
public:
    Grafo(const char* dir, const char* filename);
    string toString();
    int cost(int tag[]);
    int subCost(int tag[], int vertices[], int len);
    int flipDelta(int i, int j, int tag[]);
    static void flip(const int i, const int j, int tag[]);
    static bool IsPermutation(Solution s);
    double calculeGap(int solValue);

    int** Adj; //adj[i] = vetor de vertices adjacentes do vertice i
    set<int>* Neighbors; //set[i] = conjunto de vertices adjacentes do vertice i
    int numNodes; //número de vértices
    int numEdges; //número de arestas
    int optimal;



};

#endif // GRAFO_H
