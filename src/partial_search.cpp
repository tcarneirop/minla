
#include <limits>
#include "../headers/grafo.h"
#include "../headers/partial_search.h"
#include "../headers/full_perm.h"
#include <iostream>


int partial_search(int cutoff_depth, unsigned long long *tree_size, unsigned long long *qtd_valid_subsolutions, 
    Grafo *grafo, int *subsolution_pool, int upper_bound){

    unsigned flag = 0;
    unsigned bit_test = 0;
    int vertice[_MAX_]; //representa o ciclo
    int i, depth = 0; //para dizer que 0-1 ja foi visitado e a busca comeca de 1, bote 2
 
    unsigned long long local_tree = 0ULL;
    int best_sol = upper_bound;
    
    int current_sol = 0;
    int num_sols = 0;
    int N = grafo->numNodes;
    int partial_cost = 0;
    int partial_sol = 0;    
    int stack[_MAX_];

    /*init*/
    for (i = 0; i < N; ++i) { //
        vertice[i] = _EMPTY_;
    }

   
    while(true){ //search itself

        vertice[depth]++;
        bit_test = 0;
        bit_test |= (1<<vertice[depth]);

        if(vertice[depth] == N){ //all combinations for a given depth have been evaluated
            vertice[depth] = _EMPTY_;
        }
        else{

            if (!(flag & bit_test) ){ //is it feasible?

                if(depth == 0){
                    ++local_tree;
                    stack[depth] = 0;
                    flag |= (1ULL<<vertice[depth]);
                    depth++;
                    continue;
                } //at least two 
                

                partial_cost = grafo->ppartial_cost(vertice,depth+1);
               
                //std::cout<<"partial cost "<<partial_cost<<" "<<N<<"\n";

                if(partial_sol+partial_cost < best_sol){
                
                    //cout<<" Leng: "<< depth+1<<" Partial cost: "<< current_sol<<" Cost test: "<<partial_sol+grafo->ppartial_cost(vertice,depth+1)<<"\n";
                    flag |= (1ULL<<vertice[depth]);
                    partial_sol += partial_cost; 
                    
                    stack[depth] = partial_cost;
                    
                    depth++;
                    ++local_tree;
                    
                    if (depth == cutoff_depth){ //a complete solution 
                        ++num_sols;
                        //std::copy(vertice, vertice + N, permutation);
                        cout<<std::endl<<"Sol "<<num_sols<< " :"<<partial_sol<<" "<<std::endl;
                        for(int k = 0; k < cutoff_depth; k++){
                            std::cout   << " " << vertice[k];
                        }  
                         std::cout   << "\n ";
                    }//complete solution
                    else continue;
                }//prune by value
                else continue;
            }
            else continue;

        }//first else

        depth--; 
        
        //std::cout<<"\n alor";

        partial_sol-=stack[depth];
        flag &= ~(1ULL<<vertice[depth]);

        if(depth < 0)
            break;
        //termination condition of the search


    }//end while -- end of the enumeration

    *tree_size = local_tree;
    *qtd_valid_subsolutions = num_sols;

    return best_sol;
}