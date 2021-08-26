#ifndef RANDOMHEURISTIC_H
#define RANDOMHEURISTIC_H

//#define verbose_random

#include "grafo.h"
#include "heuristic.h"

#include <limits.h>


class RandomGenerator : public Heuristic
{
public:
    RandomGenerator(Grafo *G);
    int solve(int iter, int tagOutput[]);

    static void shuffler(int perm[], int n){
        srand(time(NULL));
        for(int i = 0 ; i < n ; i++){
            int j = rand()% n;
            int aux = perm[i];
            perm[i] = perm[j];
            perm[j] = aux;
        }
    }


private:
    int *tag = nullptr;
    int size = 0;

};

#endif // RANDOMHEURISTIC_H
