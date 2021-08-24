#ifndef LOCALSEARCH_H
#define LOCALSEARCH_H

#include "grafo.h"
#include "heuristic.h"

class LocalSearch : public Heuristic
{
public:
    LocalSearch(Grafo *g);
    int solve(int Output[], int bestCost, bool fistImprove);
    int flip2(int output[], int bestCost, bool firstImprove, int gapMin, int gapMax);
    int flip3(int output[], int bestCost, bool firstImprove, int gapMin, int gapMax);

private:
    bool verbose = false;
    int* map;
    int auxFlip3[3];
    int flipDelta(int i, int j, int k, int tag[], int var);

};

#endif // LOCALSEARCH_H
