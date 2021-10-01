#include <limits>
#include <iostream>
#include "../headers/grafo.h"
#include "../headers/full_perm.h"
#include "../headers/minla_node.h"
#include "../headers/partial_search.h"





int minla_omp_node_explorer(int cutoff_depth, unsigned long long *tree_size, unsigned long long *qtd_valid_subsolutions, 
    Grafo *grafo, Minla_node *pool, int node_id, int upper_bound){

    unsigned flag = 0;
    unsigned bit_test = 0;
    int permutation[_MAX_]; //representa o ciclo
    int i, depth; //para dizer que 0-1 ja foi visitado e a busca comeca de 1, bote 2
 
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
        permutation[i] = _EMPTY_;
    }

    depth = cutoff_depth;
    control = pool[node_id].flag;

    for(i = 0; i<cutoff_depth;++i)
        permutation[i] = pool[node_id].permutation[i];
    

 
    while(true){ //search itself

        permutation[depth]++;
        bit_test = 0;
        bit_test |= (1<<permutation[depth]);

        if(permutation[depth] == N){ //all combinations for a given depth have been evaluated
            permutation[depth] = _EMPTY_;
        }
        else{

            if (!(flag & bit_test) ){ //is it feasible?

                if(depth == 0){
                    ++local_tree;
                    stack[depth] = 0;
                    flag |= (1ULL<<permutation[depth]);
                    depth++;
                    continue;
                } //at least two 
                

                partial_cost = grafo->ppartial_cost(permutation,depth+1);

                if(partial_sol+partial_cost < best_sol){
                
                    //cout<<" Leng: "<< depth+1<<" Partial cost: "<< current_sol<<" Cost test: "<<partial_sol+grafo->ppartial_cost(permutation,depth+1)<<"\n";
                    flag |= (1ULL<<permutation[depth]);
                    partial_sol += partial_cost; 
                    
                    stack[depth] = partial_cost;
                    
                    depth++;
                    ++local_tree;
                    
                    if (depth == N){ //a complete solution 
                        
                    	++num_sols;
                        best_sol = partial_sol;
                        cout<<std::endl<<"Sol "<<num_sols<< " :"<<best_sol<<" "<<std::endl;
                        for(int k = 1; k < N; k++){
                            std::cout   << " " << vertice[k];
                        }

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
        flag &= ~(1ULL<<permutation[depth]);

        if(depth < cutoff_depth)
            break;
        //termination condition of the search

    }//end while -- end of the enumeration

    *tree_size = local_tree;
    *qtd_valid_subsolutions = num_sols;

    return best_sol;
}