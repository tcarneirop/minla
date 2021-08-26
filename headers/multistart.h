#ifndef MULTISTART_H
#define MULTISTART_H
//#define verbose_multiStart

#include "solver.h"
#include "climber.h"

#include "heuristic.h"
#include "randomgenerator.h"
#include "hillclimb.h"

class MultiStart: public Heuristic
{
public:
    MultiStart(Grafo* grafo, bool firstImprove);
    int solve(int iter, int output[], Climber *climber = 0, Solver *solver = 0);

private:
    int *tag = nullptr;
    bool firstImp = true;
};

#endif // MULTISTART_H
