#include <limits>
#include <iostream>
#include <omp.h>
#include <chrono>
#include "../headers/grafo.h"
#include "../headers/full_perm.h"
#include "../headers/minla_node.h"
#include "../headers/partial_search.h"
#include "../headers/minla_omp_search.h"


//@todo: implement the atomic coherency of the incumbent solution.
//@todo: retrieve the permutation in parallel
//@todo: in the search: verify and update upper bound

void progress_bar(int upper_bound, int qtd_sol, int nodes_evaluated, int pool_size, unsigned long long partial_tree){

    float percent = (float)nodes_evaluated/(float)pool_size;
    int interval = (percent*100);

    std::cout<<" [ ";

    for(int x = 0; x<interval-1; ++x){
        std::cout<<"|";
    }

    std::cout<<"] \n";
  //  elapsed = getCurrentTime() - start_chkpt;
    std::cout<<interval<<"% # Nodes remaining: "<< pool_size-nodes_evaluated<< " # < Partial tree: " << partial_tree<<" > \n"; 
    //<<" > < Elapsed time: ",  elapsed," (s) > #\en";
    ///////////
}

void minla_call_omp_search(int cutoff_depth, Grafo *grafo, int upper_bound){

    int pool_size = 0; 
    int nodes_evaluated = 0;
    int qtd_sol = 0;
    unsigned long long initial_search_tree_size = 0ULL;
    unsigned long long final_search_tree_size = 0ULL;
    int best_sol = upper_bound;
    float limit = 0.05;
    std::cout.precision(5);

    std::cout <<"\n Partial search -  Cutoff depth: " << cutoff_depth<<"\n";

    std::chrono::steady_clock clk;   // create an object of `steady_clock` class
    
    auto start = clk.now();

    Minla_node *subsolutions_pool = minla_start_pool(grafo, cutoff_depth);
    minla_partial_search(cutoff_depth, &initial_search_tree_size, &qtd_sol, grafo, subsolutions_pool, upper_bound);
    pool_size = qtd_sol;
    minla_print_pool(subsolutions_pool, pool_size, cutoff_depth);
    
    std::cout<<"Maximum pool size: "<<minla_max_pool_size(grafo,cutoff_depth)<<"\n";
    std::cout<<std::endl<<std::endl<<"Pool size: "<<pool_size<<"\n";

    qtd_sol = 0;
    #pragma omp parallel for default(none) firstprivate(best_sol) shared(std::cout,limit,nodes_evaluated,upper_bound,subsolutions_pool,grafo,cutoff_depth,pool_size) schedule(runtime) reduction(+:final_search_tree_size, qtd_sol)
    for(auto subsol = 0; subsol<pool_size;++subsol){

        unsigned long long local_tree_size = 0ULL;
        int local_qtd_sol = 0;
        best_sol = minla_omp_node_explorer(cutoff_depth, &local_tree_size, &local_qtd_sol, grafo, subsolutions_pool, subsol, upper_bound);

        #pragma omp critical
        {

            final_search_tree_size += local_tree_size;
            qtd_sol += local_qtd_sol;
            ++nodes_evaluated;
            if(best_sol<upper_bound){
                upper_bound = best_sol;
                std::cout<<"New solution found: "<<upper_bound<<"\n";
            }
            if((float)nodes_evaluated/(float)pool_size > limit){
                limit=(limit>0.9 ? 0.9 : limit*1.1);
                progress_bar(upper_bound,qtd_sol,nodes_evaluated,pool_size, final_search_tree_size);
            }
        }

    }

    auto end = clk.now();       // end timer (starting & ending is done by measuring the time at the moment the process started & ended respectively)
    auto time_span = static_cast<std::chrono::duration<double>>(end - start);   // measure time span between start & end

    unsigned long long total_tree = initial_search_tree_size+final_search_tree_size;
    std::cout<<"Number of solutions found: "<<qtd_sol<<"\n\tOptimal solution: "<<upper_bound<<"\n\n";
    std::cout<<"Tree size: "<<total_tree<<"\n";
    std::cout<<fixed<<"Performance: "<<total_tree/time_span.count()<< " nodes/sec\n";
    std::cout<<"\nElapsed time: "<< time_span.count() <<" seconds"<<"\n";

}

int minla_omp_node_explorer(int cutoff_depth, unsigned long long *tree_size, int *qtd_sols, 
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
    int stride = N-1;

    for (i = 0; i < N; ++i) { //
        permutation[i] = _EMPTY_;
    }

    depth = cutoff_depth;
    flag = pool[node_id].flag;
    partial_sol = pool[node_id].cost;

    //for(i = 0; i<cutoff_depth;++i)
    //    permutation[i] = pool[node_id].permutation[i];

    std::copy(pool[node_id].permutation, pool[node_id].permutation+cutoff_depth, permutation);

    
    while(true){ //search itself

        permutation[depth]++;
        bit_test = 0;
        bit_test |= (1<<permutation[depth]);

        if(permutation[depth] == N){ //all combinations for a given depth have been evaluated
            permutation[depth] = _EMPTY_;
        }
        else{

            if (!(flag & bit_test) ){ //is it valid?

                partial_cost = grafo->ppartial_cost(permutation,depth+1);
                //partial_cost = v_partial_cost(permutation,depth+1, grafo->gpu_adj_list, grafo->gpu_size_adj_list, stride);


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
                        // #pragma omp critical 
                        // {
                        //     cout<<std::endl<<"Sol "<<num_sols<< " :"<<best_sol<<" "<<std::endl;
                        //     for(int k = 1; k < N; k++){
                        //         std::cout   << " " << permutation[k];
                        //     }

                        // }

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

    *tree_size += local_tree;
    *qtd_sols += num_sols;

    return best_sol;
}