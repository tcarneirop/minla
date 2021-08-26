#include "../headers/multistart.h"

MultiStart::MultiStart(Grafo* grafo, bool firstImprove): Heuristic(grafo), firstImp(firstImprove){
    tag = new int[grafo->numNodes];
}

///aqui considero que um ponto inicial vai ser passado para o multistart
int MultiStart::solve(int iter, int output[], Climber *climber, Solver *solver){
    int bestCost = INT_MAX;

    std::copy(output, output+grafo->numNodes, tag); //copia output para tag

    if(solver && climber){
        for(int i = 0; i < iter; i++){
            int c = climber->climb(tag, solver->solve(tag))   ; //depende do solver e do climber passado por parâmetro
            if(c < bestCost){
                bestCost = c;
                std::copy(tag, tag+grafo->numNodes, output); //copia tag para output

                #ifdef verbose_multiStart
                    cout << "iter: " << i << " Cost:" << bestCost  << endl;
                #endif
            }
        }
    }else{
        cerr << " ERROR: método solver ou climber não definido." << endl;
        exit(1);
    }

    return bestCost;
}

