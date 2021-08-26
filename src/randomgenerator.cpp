#include "../headers/randomgenerator.h"

int myrandom (int i) { return std::rand()%i;}

RandomGenerator::RandomGenerator(Grafo *G):Heuristic(G)
{
    size = grafo->numNodes;
    tag = new int[size];
    for(int i = 0; i < size; i++) tag[i] = i+1;
}

int RandomGenerator::solve(int iter, int tagOutput[]){

#ifdef verbose_random
    cout << "### Random Solver started ###" << endl;
#endif

    int newCost, bestCost = INT_MAX;

    srand(time(0));

    for(int i = 0; i < iter; i++){

        random_shuffle(tag, tag+size);

        newCost = grafo->cost(tag);

        if(newCost < bestCost){
            bestCost = newCost;
            std::copy(tag, tag+size, tagOutput);

            #ifdef verbose_random
                cout << "Iteração:" << i <<" - Cost:"<< bestCost << endl;
            #endif
        }
    }

    return bestCost;
}
