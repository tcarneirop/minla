
#include <limits>
#include "../headers/grafo.h"
#include "../headers/full_perm.h"
#include "../headers/backtracking.h"
#include <iostream>

int BP_backtracking_serial(unsigned long long *tree_size, unsigned long long *qtd_sol, Grafo *grafo, int *permutation, int best_sol){

    unsigned flag = 0;
    unsigned bit_test = 0;
    int vertice[_MAX_]; //representa o ciclo
    int i, nivel; //para dizer que 0-1 ja foi visitado e a busca comeca de 1, bote 2
    unsigned cont  = 0;
    unsigned long long local_tree = 0ULL;
    int current_sol = 0;
    int num_sols = 0;
    int N = grafo->numNodes+1;
    int vertex_lb;
   
    /*init*/
    for (i = 0; i < N; ++i) { //
        vertice[i] = -1;
        //maybe we need here 0 instead of -1?
    }


    vertice[0] = 0;
    flag |= (1ULL<<0);
    nivel = 1;

    while(true){ //search itself

        vertice[nivel]++;
        bit_test = 0;
        bit_test |= (1<<vertice[nivel]);

        if(vertice[nivel] == N){ //all combinations for a given depth have been evaluated
            vertice[nivel] = _EMPTY_;
        }
        else{

            if (!(flag & bit_test) ){ //is it feasible?

                flag |= (1ULL<<vertice[nivel]);
                nivel++;
                ++local_tree;

                

                if (nivel == N){ //a complete solution

                     ++num_sols;
                    current_sol = grafo->cost((vertice+1));

                    if(current_sol<best_sol){ //Improves the best sol?
                        best_sol = current_sol;
                        std::copy(vertice+1, vertice + N, permutation);
                        cout<<std::endl<<"Sol "<<num_sols<< " :"<<best_sol<<" "<<std::endl;
                        for(int k = 1; k < N; k++){
                            std::cout   << " " << vertice[k];
                        }
                    }//if best sol

                }//a complete solution
                else
                    continue;
            }
            else //no, not feasible
                continue;

        }//first else

        nivel--; 
        flag &= ~(1ULL<<vertice[nivel]);

        if(nivel <= 0)
            break;
        //termination condition of the search


    }//end while -- end of the enumeration


    *tree_size = local_tree;
    *qtd_sol = num_sols;

    return best_sol;
}




