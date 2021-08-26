#ifndef GREEDYADAPTATIVEGEN_H
#define GREEDYADAPTATIVEGEN_H

#include "grafo.h"
#include "vector"
#include "randomgenerator.h"
#include "solver.h"

class GreedyAdaptativeGen : public Solver
{
public:
    Grafo *grafo;
    vector< pair<int, int> > vertice;
    int *aux;
    int tol= 0;

    GreedyAdaptativeGen(Grafo *grafo, int tolerancia);
    int solve(int output[]); //método implementado da interface solver
};

#endif // GREEDYADAPTATIVEGEN_H
