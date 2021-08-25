#include "../headers/localsearch.h"

LocalSearch::LocalSearch(Grafo *g):Heuristic(g){
    map = new int[g->numNodes];

    int i = {0};
    std::generate(map, map+g->numNodes, [&i]{ return i++; });

    random_shuffle(map, map+g->numNodes);
}

inline void flip(int i,int j, int tag[]){
    int t = tag[i];
    tag[i] = tag[j];
    tag[j] = t;
}

int LocalSearch::solve(int Output[], int bestCost, bool firstImprove){

    verbose = true;
    if(verbose){
        cout << "### Local Search started ###" << endl;
        if(firstImprove)
            cout << "### Using first improve  ###" << endl;
    }

    //int bc = grafo->cost(Output);
    int argI = -1, argJ = -1, min = 0;

    bool exit = false;

    for(int i = 0 ; i < grafo->numNodes; i++ ){
        for(int j = i+1 ; j < grafo->numNodes; j++){
           int delta =  grafo->flipDelta(i,j,Output);
            if( delta < min){
                min = delta;
                argI = i;
                argJ = j;

                if(verbose){
                    cout << " Local Search Cost:"<< bestCost + delta << endl;
                    
                }

                if(firstImprove){
                    exit = true;
                }
            }
        }

        if(exit) break;
    }

    if(argI >= 0)
        flip(argI, argJ, Output);

    if(verbose){
        cout << " Finished" << endl;
    }

    return bestCost + min;
}

int LocalSearch::flip2(int output[], int bestCost, bool firstImprove, int gapMin, int gapMax){
    if(verbose){
        if(firstImprove){
            cout << "### Flip2 started (first Improve)###" << endl;
        }else{
            cout << "### Flip2 Search started ###" << endl;
        }
    }

    bool sair = false;
    int argI = -1, argJ = -1, min = 0;
    random_shuffle(map,map+grafo->numNodes); //embaralha o map

    for(int ii = 0; ii < grafo->numNodes; ii++){
        for(int jj = ii+1; jj < grafo->numNodes; jj++){
            int tmp = abs(output[map[ii]] - output[map[jj]]);
            if(tmp <= gapMax && tmp >= gapMin){
                int i = map[ii];
                int j = map[jj];
                int delta = grafo->flipDelta(i,j,output);
                if(delta < min){
                    min = delta;
                    argI = i;
                    argJ = j;

                    if(verbose){
                        cout << "cost: " << bestCost + delta << endl;
                    }

                    if(firstImprove){
                        sair = true;
                        break;
                    }

                }
            }
        }
        if(sair)
            break;
    }
    if(argI >= 0){
        flip(argI, argJ, output);
    }
    if(verbose){
        cout << "### Done ###" << endl;
    }

    return bestCost + min;
}

int LocalSearch::flip3(int output[], int bestCost, bool firstImprove, int gapMin, int gapMax){
    if(verbose){
        if(firstImprove){
            cout << "### Flip3 (first Improve) started ###" << endl;
        }else{
            cout << "### Flip3 Search started ###" << endl;
        }
    }

    int argI=-1, argJ=-1, argK=-1, argV=-1, min = 0;
    random_shuffle(map, map+grafo->numNodes);

    for(int ii = 0; ii < grafo->numNodes; ii++){
        for(int jj = ii + 1; jj < grafo->numNodes; jj++){
            int gap = abs( output[map[ii]] - output[map[jj]] );
            if(gap <= gapMax && gap >= gapMin){
                for(int kk = jj + 1; kk < grafo->numNodes; kk++){
                    gap = abs( output[map[ii]] - output[map[kk]] );
                    int gap2 = abs( output[map[jj]] - output[map[kk]] );
                    if(gap <= gapMax && gap2 <= gapMax && gap >= gapMin && gap2 >= gapMin){
                        for(int v = 0; v < 2; v++){
                            int i = map[ii];
                            int j = map[jj];
                            int k = map[kk];
                            int delta = flipDelta(i,j,k, output,v);
                            if(delta < min){
                                min = delta;
                                argI = i;
                                argJ = j;
                                argK = k;
                                argV = v;

                                if(verbose){
                                    cout << "cost: " << bestCost + delta << endl;
                                }

                                if(firstImprove){
                                    v = 2;
                                    ii = jj = kk = grafo->numNodes+1; //gambis, ver com resolver
                                }
                            }

                        }

                    }

                }
            }
        }
    }

    if(argI >= 0){
        if(argV == 0){
            int aux = output[argI];
            output[argI] = output[argK];
            output[argK] = output[argJ];
            output[argJ] = aux;
        }else{
            int aux = output[argI];
            output[argI] = output[argJ];
            output[argJ] = output[argK];
            output[argK] = aux;
        }
    }

    if(verbose){
        cout << "### DONE ###";
    }

    return bestCost + min; //custo da solução melhorada

}


int LocalSearch::flipDelta(int i, int j, int k, int tag[], int var){
    auxFlip3[0] = i;
    auxFlip3[1] = j;
    auxFlip3[2] = k;

    int delta = grafo->subCost(tag, auxFlip3, 3);

    if(var == 0){
        //ijk -> kij
        int aux = tag[i];
        tag[i] = tag[k];
        tag[k] = tag[j];
        tag[j] = aux;

        delta = grafo->subCost(tag, auxFlip3, 3) - delta;

        //kij -> ijk
        aux = tag[k];
        tag[k] = tag[i];
        tag[i] = tag[j];
        tag[j] = aux;
    }else{
        //ijk -> jki
        int aux = tag[i];
        tag[i] = tag[j];
        tag[j] = tag[k];
        tag[k] = aux;

        delta = grafo->subCost(tag, auxFlip3, 3) - delta;

        //jki -> ijk
        aux = tag[j];
        tag[j] = tag[i];
        tag[i] = tag[k];
        tag[k] = aux;
    }

    return delta;
}
