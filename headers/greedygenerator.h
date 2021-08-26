#ifndef GREEDYGENERATOR_H
#define GREEDYGENERATOR_H

#include "grafo.h"
#include "vector"

using namespace std;

class GreedyGenerator
{
public:
    Grafo *grafo;
    vector< pair<int, int> > vertice;

    GreedyGenerator(Grafo *grafo);
    int solve(int output[]);

};

#endif // GREEDYGENERATOR_H
