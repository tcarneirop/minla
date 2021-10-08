#include <limits>
#include <iostream>
#include <omp.h>
#include <cuda.h
>#include <chrono>
#include "../headers/grafo.h"
#include "../headers/full_perm.h"
#include "../headers/minla_node.h"
#include "../headers/partial_search.h"
#include "../headers/minla_omp_search.h"
#include "../headers/minla_gpu_search.h"

#define _MINLA_BLOCK_SIZE_     128

#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true)
{
   if (code != cudaSuccess) 
   {
      fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
      if (abort) exit(code);
   }
}

void minla_call_cuda_search(int cutoff_depth, Grafo *grafo, int upper_bound){

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

    qtd_sol = 0;
    
    auto end = clk.now();       // end timer (starting & ending is done by measuring the time at the moment the process started & ended respectively)
    auto time_span = static_cast<std::chrono::duration<double>>(end - start);   // measure time span between start & end

    unsigned long long total_tree = initial_search_tree_size+final_search_tree_size;
    std::cout<<"Number of solutions found: "<<qtd_sol<<"\n\tOptimal solution: "<<upper_bound<<"\n\n";
    std::cout<<"Tree size: "<<total_tree<<"\n";
    std::cout<<fixed<<"Performance: "<<total_tree/time_span.count()<< " nodes/sec\n";
    std::cout<<"\nElapsed time: "<< time_span.count() <<" seconds"<<"\n";

}


int GPU_minla_call_multigpu_kernel(int gpu_id, int cutoff_depth, unsigned long long *local_tree_size, int *qtd_sol, Grafo *grafo, int upper_bound, int pool_size,  Minla_node *subsolutions_pool_h){
    
    cudaSetDevice(gpu_id);
   // cudaFuncSetCacheConfig(BP_queens_root_dfs,cudaFuncCachePreferL1);
   

    unsigned long long *vector_of_tree_size_d;
    int *qtd_sols_d;
    Minla_node *subsolutions_pool_d;

    int num_blocks = ceil((double)pool_size/_MINLA_BLOCK_SIZE_);

    cudaMalloc((void**) &vector_of_tree_size_d,pool_size*sizeof(unsigned long long));
    cudaMalloc((void**) &qtd_sols_d,pool_size*sizeof(int));
    cudaMalloc((void**) &subsolutions_pool_d,pool_size*sizeof(Minla_node));


    cudaMemcpy(subsolutions_pool_d, subsolutions_pool_h, pool_size * sizeof(Minla_node), cudaMemcpyHostToDevice);

    std::cout<<"### Regular BP-DFS search. ###\n";
    
    
    BP_queens_root_dfs<<< num_blocks,_QUEENS_BLOCK_SIZE_>>> (size,n_explorers,initial_depth,root_prefixes_d, vector_of_tree_size_d,sols_d);
    gpuErrchk( cudaDeviceSynchronize() );
    gpuErrchk( cudaPeekAtLastError() );
  

    cudaMemcpy(vector_of_tree_size_h,vector_of_tree_size_d,pool_size*sizeof(unsigned long long),cudaMemcpyDeviceToHost);
    cudaMemcpy(qtd_sols_h,qtd_sols_d,pool_size*sizeof(int),cudaMemcpyDeviceToHost);

    for(int i = 0; i<pool_size;++i){
        local_tree_size+=vector_of_tree_size_h[i];
        qtd_sols+=qtd_sols_h[i];
    }

    cudaFree(vector_of_tree_size_d);
    cudaFree(qtd_sols_d);
    cudaFree(subsolutions_pool_d);
    //After that, Chapel reduces the values
}
