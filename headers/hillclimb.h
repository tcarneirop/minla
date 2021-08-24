#ifndef HILLCLIMB_H
#define HILLCLIMB_H

//#define verbose_hillclimb

#include "grafo.h"
#include "heuristic.h"
#include "localsearch.h"
#include "solver.h"
#include <limits.h>

class HillClimb : public Heuristic, public Solver
{
public:
    HillClimb(Grafo *G);
    void findInitialPoint(int tagOutPut[], int n);
    int solve(int tagOutput[]); //método implementado da interface solver


private:
    LocalSearch localsearch;
};

#endif // HILLCLIMB_H
