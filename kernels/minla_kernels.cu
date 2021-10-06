#include <limits>
#include <iostream>
#include <cuda.h>
#include <omp.h>
#include <chrono>
#include "../headers/grafo.h"
#include "../headers/full_perm.h"
#include "../headers/minla_node.h"
#include "../headers/minla_kernels.h"

#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true)
{
   if (code != cudaSuccess) 
   {
      fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
      if (abort) exit(code);
   }
}

__global__ void minla_gpu_node_explorer(int cutoff_depth, unsigned long long *tree_size, int *qtd_sols, int *best_sols,
    int* adj_list, Minla_node *pool, int pool_size,int upper_bound){

    int idx = blockIdx.x * blockDim.x + threadIdx.x;
     if(idx<pool_size){ //idx check

    //     unsigned flag = 0;
    //     unsigned bit_test = 0;
    //     int permutation[_MAX_]; //representa o ciclo
    //     int i, depth; //para dizer que 0-1 ja foi visitado e a busca comeca de 1, bote 2
     
    //     unsigned long long local_tree = 0ULL;
    //     int best_sol = upper_bound;
        
    //     int current_sol = 0;
    //     int num_sols = 0;
    //     int N = grafo->numNodes;
    //     int partial_cost = 0;
    //     int partial_sol = 0;    
    //     int stack[_MAX_];

    //     for (i = 0; i < N; ++i) { //
    //         permutation[i] = _EMPTY_;
    //     }

    //     depth = cutoff_depth;
    //     flag = pool[node_id].flag;
    //     partial_sol = pool[node_id].cost;

    //     for(i = 0; i<cutoff_depth;++i)
    //         permutation[i] = pool[node_id].permutation[i];
        
    //     while(true){ //search itself

    //         permutation[depth]++;
    //         bit_test = 0;
    //         bit_test |= (1<<permutation[depth]);

    //         if(permutation[depth] == N){ //all combinations for a given depth have been evaluated
    //             permutation[depth] = _EMPTY_;
    //         }
    //         else{

    //             if (!(flag & bit_test) ){ //is it valid?

    //                 partial_cost = grafo->ppartial_cost(permutation,depth+1);

    //                 if(partial_sol+partial_cost < best_sol){
                    
    //                     //cout<<" Leng: "<< depth+1<<" Partial cost: "<< current_sol<<" Cost test: "<<partial_sol+grafo->ppartial_cost(permutation,depth+1)<<"\n";
    //                     flag |= (1ULL<<permutation[depth]);
    //                     partial_sol += partial_cost; 
                        
    //                     stack[depth] = partial_cost;
                        
    //                     depth++;
    //                     ++local_tree;
                        
    //                     if (depth == N){ //a complete solution 
                            
    //                         ++num_sols;
    //                         best_sol = partial_sol;
    //                         //cout<<std::endl<<"Sol "<<num_sols<< " :"<<best_sol<<" "<<std::endl;
    //                         //for(int k = 1; k < N; k++){
    //                         //    std::cout   << " " << permutation[k];
    //                         //}

    //                     }//complete solution
    //                     else continue;
    //                 }//prune by value
    //                 else continue;
    //             }
    //             else continue;

    //         }//first else

    //         depth--; 
            
    //         //std::cout<<"\n alor";

    //         partial_sol-=stack[depth];
    //         flag &= ~(1ULL<<permutation[depth]);

    //         if(depth < cutoff_depth)
    //             break;
    //         //termination condition of the search

    //     }//end while -- end of the enumeration

        tree_size[idx] = 1;
        qtd_sols[idx]  = 1;
        best_sols[idx]  = 1;


    }//if idx   
 
}



void minla_call_multigpu_kernel(int gpu_id, int cutoff_depth, unsigned long long *tree_size,
    int *qtd_sols, Grafo *grafo, int upper_bound, int pool_size,  Minla_node *subsolutions_pool_h){
    
    cudaSetDevice(gpu_id);
   // cudaFuncSetCacheConfig(BP_queens_root_dfs,cudaFuncCachePreferL1);
   
    unsigned long long *vector_of_tree_size_d;
    int *qtd_sols_d, *best_sol_d, *adj_list_d;
    
    Minla_node *subsolutions_pool_d;

    int local_qtd_sols = 0;
    unsigned long long local_tree_size = 0ULL;


    int num_blocks = ceil((double)pool_size/_MINLA_BLOCK_SIZE_);

    /////////////////////
    ////Host buffer
    ////////////////////
    int *qtd_sols_h =  (int *) malloc (sizeof(int)*pool_size);
    int *best_sol_h =  (int *) malloc (sizeof(int)*pool_size);
    int *adj_list_h =  (int *) malloc (sizeof(int)*grafo->numNodes*grafo->numNodes);
    unsigned long long *vector_of_tree_size_h = (unsigned long long *) malloc (sizeof(unsigned long long)*pool_size);


    /////////////////////
    ////GPU Buffer
    ////////////////////

    cudaMalloc((void**) &vector_of_tree_size_d, pool_size*sizeof(unsigned long long));
    cudaMalloc((void**) &adj_list_d, sizeof(int)*grafo->numNodes*grafo->numNodes);
    cudaMalloc((void**) &qtd_sols_d,pool_size*sizeof(int));
    cudaMalloc((void**) &best_sol_d, pool_size*sizeof(int));
    cudaMalloc((void**) &subsolutions_pool_d,pool_size*sizeof(Minla_node));

    //memcopy 

    cudaMemcpy(subsolutions_pool_d, subsolutions_pool_h, pool_size * sizeof(Minla_node), cudaMemcpyHostToDevice);

    std::cout<<"### Regular BP-DFS search. ###\n";
    
    minla_gpu_node_explorer<<< num_blocks,_MINLA_BLOCK_SIZE_>>> (cutoff_depth, vector_of_tree_size_d, qtd_sols_d, best_sol_d,
        adj_list_d, subsolutions_pool_d, pool_size,upper_bound);   
 
    gpuErrchk( cudaDeviceSynchronize() );
    gpuErrchk( cudaPeekAtLastError() );
    std::cout<<"### End of the GPU search ###\n";

    cudaMemcpy(vector_of_tree_size_h,vector_of_tree_size_d,pool_size*sizeof(unsigned long long),cudaMemcpyDeviceToHost);
    cudaMemcpy(qtd_sols_h,qtd_sols_d,pool_size*sizeof(int),cudaMemcpyDeviceToHost);

    for(int i = 0; i<pool_size;++i){
        local_tree_size += vector_of_tree_size_h[i];
        local_qtd_sols  += qtd_sols_h[i];
    }

    std::cout<<"Local tree size: "<<local_tree_size<<"\n";
    std::cout<<"qtd_sols: "<<local_qtd_sols<<"\n";

    cudaFree(vector_of_tree_size_d);
    cudaFree(qtd_sols_d);
    cudaFree(subsolutions_pool_d);
    //After that, Chapel reduces the values
}
