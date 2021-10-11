
#include <limits>
#include <iostream>
#include "../headers/grafo.h"
#include "../headers/full_perm.h"
#include "../headers/minla_node.h"
#include "../headers/partial_search.h"

int minla_max_pool_size(Grafo *grafo,int cutoff_depth) {
    int  x = 1;
    int N = grafo->numNodes;
    int i;

    if(cutoff_depth==1)
        return N;
    for (i = 0; i < cutoff_depth; ++i) {
        x *= (N - i);
    }
    return x;
}

void minla_print_pool(Minla_node *pool, int pool_size, int cutoff_depth){

    std::cout<<"Minla pool of size: "<<pool_size<<"\n";
    for(int i = 0; i<pool_size;++i){
        std::cout<<"Node: "<<i<<"\n";
        for(int j = 0; j<cutoff_depth; ++j ){
            std::cout<<pool[i].permutation[j]<<" ";
        }//
        std::cout<<"flag: "<<pool[i].flag<<" "<<"Cost: "<<pool[i].cost<<"\n";
    }//

}//print pool

Minla_node* minla_start_pool(Grafo *grafo, int cutoff_depth){

    if(cutoff_depth>_MAX_DEPTH_){
        std::cout<<"Cutoff depth bigger than the maximum depth defined in minlanode.h: "<<_MAX_DEPTH_<<".\n";
        exit(1);
    }

    unsigned int pool_size = minla_max_pool_size(grafo, cutoff_depth);
    Minla_node* pool = static_cast<Minla_node*>(malloc(pool_size*sizeof(Minla_node)));

    if (pool == NULL) {
        std::cout<<"Fatal: failed to allocate " << pool_size <<" nodes of size "<<sizeof(Minla_node)<<" bytes ( "<<pool_size*sizeof(Minla_node)/1000000000<<" GB ).\n";
        exit(1);
    }

    std::cout<<"\nPool of size "<<pool_size<<" - "<<pool_size*sizeof(Minla_node)/1000000<<"(MB) allocated.\n";
    return pool;
}


int minla_partial_search(int cutoff_depth, unsigned long long *tree_size, int *qtd_valid_subsolutions, 
    Grafo *grafo, Minla_node *pool, int upper_bound){

    unsigned flag = 0;
    unsigned bit_test = 0;
    int permutation[_MAX_]; //representa o ciclo
    int i, depth = 0; //para dizer que 0-1 ja foi visitado e a busca comeca de 1, bote 2
 
    unsigned long long local_tree = 0ULL;
    int best_sol = upper_bound;
    
    int current_sol = 0;
    int num_sols = 0;
    int N = grafo->numNodes;
    int partial_cost = 0;
    int partial_sol = 0;    
    int stack[_MAX_];

    for (i = 0; i < N; ++i)
        permutation[i] = _EMPTY_;
    
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
                    
                    if (depth == cutoff_depth){ //a complete solution 
                        
                        pool[num_sols].flag = flag;
                        pool[num_sols].cost = partial_sol;
                        std::copy(permutation, permutation+cutoff_depth, pool[num_sols].permutation);
                        ++num_sols;

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

        if(depth < 0)
            break;
        //termination condition of the search

    }//end while -- end of the enumeration

    *tree_size = local_tree;
    *qtd_valid_subsolutions = num_sols;

    return best_sol;
}