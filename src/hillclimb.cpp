

#include "../headers/hillclimb.h"
#include "../headers/grafo.h"

HillClimb::HillClimb(Grafo *G): Heuristic(G), localsearch(G){}


void HillClimb::findInitialPoint(int tagOutPut[], int n){
    for(int j = 0 ; j < n ; j++)
        tagOutPut[j] = j+1;
    random_shuffle(tagOutPut, tagOutPut+n);
}

int HillClimb::solve(int tagOutput[]){

    int c, count = 0;
    int nc = grafo->cost(tagOutput);
    do{
        c = nc;
        nc = localsearch.solve(tagOutput, nc, false);
    }while(nc < c);

    return nc;

}

