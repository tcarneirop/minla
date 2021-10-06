#include <limits>
#include <iostream>
#include <omp.h>
#include <chrono>
#include "../headers/grafo.h"
#include "../headers/full_perm.h"
#include "../headers/minla_node.h"
#include "../headers/partial_search.h"
#include "../headers/minla_gpu_search.h"
#include "../headers/minla_kernels.h"

void minla_call_gpu_search(int cutoff_depth, Grafo *grafo, int upper_bound){

    int pool_size = 0; 
    int qtd_sol = 0;

    unsigned long long initial_search_tree_size = 0ULL;
    unsigned long long final_search_tree_size = 0ULL;
    int best_sol = upper_bound;

    std::cout.precision(5);

    std::cout <<"\n Partial search -  Cutoff depth: " << cutoff_depth<<"\n";

    std::chrono::steady_clock clk;   // create an object of `steady_clock` class
    
    auto start = clk.now();

    Minla_node *subsolutions_pool = minla_start_pool(grafo, cutoff_depth);
    minla_partial_search(cutoff_depth, &initial_search_tree_size, &qtd_sol, grafo, subsolutions_pool, upper_bound);
    pool_size = qtd_sol;
   // minla_print_pool(subsolutions_pool, pool_size, cutoff_depth);
    std::cout<<"Maximum pool size: "<<minla_max_pool_size(grafo,cutoff_depth)<<"\n";
    std::cout<<std::endl<<std::endl<<"Pool size: "<<pool_size<<"\n";
    pool_size = qtd_sol;
    qtd_sol = 0;

    minla_call_multigpu_kernel(0, cutoff_depth, &final_search_tree_size,&qtd_sol, grafo, upper_bound, pool_size,  subsolutions_pool);

    auto end = clk.now();       // end timer (starting & ending is done by measuring the time at the moment the process started & ended respectively)
    auto time_span = static_cast<std::chrono::duration<double>>(end - start);   // measure time span between start & end

    unsigned long long total_tree = initial_search_tree_size+final_search_tree_size;
    std::cout<<"Number of solutions found: "<<qtd_sol<<"\n\tOptimal solution: "<<upper_bound<<"\n\n";
    std::cout<<"Tree size: "<<total_tree<<"\n";
    std::cout<<fixed<<"Performance: "<<total_tree/time_span.count()<< " nodes/sec\n";
    std::cout<<"\nElapsed time: "<< time_span.count() <<" seconds"<<"\n";

}