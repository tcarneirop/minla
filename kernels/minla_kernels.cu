#include <limits>
#include <iostream>
#include <cuda.h>
#include <omp.h>
#include <chrono>
#include "../headers/grafo.h"
#include "../headers/fullperm.h"
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

void minla_call_multigpu_kernel(int gpu_id, int cutoff_depth, unsigned long long *local_tree_size,
    int *qtd_sols, Grafo *grafo, int upper_bound, int pool_size,  Minla_node *subsolutions_pool_h){
    
    cudaSetDevice(gpu_id);
   // cudaFuncSetCacheConfig(BP_queens_root_dfs,cudaFuncCachePreferL1);
   
    unsigned long long *vector_of_tree_size_d;
    int *qtd_sols_d, *best_sol_d;
    Minla_node *subsolutions_pool_d;

    int num_blocks = ceil((double)pool_size/_MINLA_BLOCK_SIZE_);

    
    unsigned long long *vector_of_tree_size_h = (unsigned long long *) malloc (sizeof(unsigned long long)*pool_size);
    int *qtd_sols_h =  (int *) malloc (sizeof(int)*pool_size);
    int *best_sol_h =  (int *) malloc (sizeof(int)*pool_size);

    cudaMalloc((void**) &vector_of_tree_size_d,pool_size*sizeof(unsigned long long));
    cudaMalloc((void**) &qtd_sols_d,pool_size*sizeof(int));
    cudaMalloc((void**) &best_sol_d, pool_size*sizeof(int));
    cudaMalloc((void**) &subsolutions_pool_d,pool_size*sizeof(Minla_node));

    cudaMemcpy(subsolutions_pool_d, subsolutions_pool_h, pool_size * sizeof(Minla_node), cudaMemcpyHostToDevice);

    std::cout<<"### Regular BP-DFS search. ###\n";
    
    
    //BP_queens_root_dfs<<< num_blocks,_QUEENS_BLOCK_SIZE_>>> (size,n_explorers,initial_depth,root_prefixes_d, vector_of_tree_size_d,sols_d);
    gpuErrchk( cudaDeviceSynchronize() );
    gpuErrchk( cudaPeekAtLastError() );
    std::cout<<"### End of the GPU search ###\n";

    cudaMemcpy(vector_of_tree_size_h,vector_of_tree_size_d,pool_size*sizeof(unsigned long long),cudaMemcpyDeviceToHost);
    cudaMemcpy(qtd_sols_h,qtd_sols_d,pool_size*sizeof(int),cudaMemcpyDeviceToHost);

    for(int i = 0; i<pool_size;++i){
        local_tree_size+=vector_of_tree_size_h[i];
        *qtd_sols+=qtd_sols_h[i];
    }

    cudaFree(vector_of_tree_size_d);
    cudaFree(qtd_sols_d);
    cudaFree(subsolutions_pool_d);
    //After that, Chapel reduces the values
}

__global__ void minla_gpu_node_explorer(int cutoff_depth, unsigned long long *tree_size, int *qtd_sols, int *best_sol,
    int** adj_list, Minla_node *pool, int upper_bound, int pool_size){

    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if(idx<pool_size){ //idx check

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

        for (i = 0; i < N; ++i) { //
            permutation[i] = _EMPTY_;
        }

        depth = cutoff_depth;
        flag = pool[node_id].flag;
        partial_sol = pool[node_id].cost;

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

                if (!(flag & bit_test) ){ //is it valid?

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
                            //cout<<std::endl<<"Sol "<<num_sols<< " :"<<best_sol<<" "<<std::endl;
                            //for(int k = 1; k < N; k++){
                            //    std::cout   << " " << permutation[k];
                            //}

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

        tree_size[idx] = local_tree;
        qtd_sols[idx]  = num_sols;
        best_sol[idx]  = best_sol;


    }//if idx   
 
}