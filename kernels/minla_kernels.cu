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

#define _GPU_ADJ(v,j) gpu_adj_list[(v)*stride+(j)]

#define _CONST_GPU_ADJ(v,j) const_gpu_adj_list[(v)*const_stride+(j)]

__constant__ int const_gpu_adj_list[_MAX_*_MAX_];
__constant__ int const_gpu_size_adj_list[_MAX_];
__constant__ int const_N;
__constant__ int const_stride;


inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true)
{
   if (code != cudaSuccess) 
   {
      fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
      if (abort) exit(code);
   }
}



__device__ inline int minla_gpu_Abs(int v){

    int const mask = v >> sizeof(int) * CHAR_BIT - 1;
    return (v + mask) ^ mask;
}

__device__ inline int minla_gpu_partial_cost(int *tag,int len, int *gpu_adj_list, int *gpu_size_adj_list, int stride){
    
    int sum = 0;
    int pos = len-1;
    int tag_pos = tag[pos];
    
    for(int j = 0 ; j < gpu_size_adj_list[pos]; j++){ //neighborhood of the vertex  

        if( _GPU_ADJ(pos,j) >= len) //not yet in the permutation
              continue;            
        
        sum += minla_gpu_Abs(tag_pos - tag[ _GPU_ADJ(pos,j) ]);
            
    }//for
    return sum;

}/////////////////////////////


__device__ inline int minla_const_gpu_partial_cost(int *tag,int len){
    
    int sum = 0;
    int pos = len-1;
    int tag_pos = tag[pos];
    
    for(int j = 0 ; j < const_gpu_size_adj_list[pos]; j++){ //neighborhood of the vertex  

        if( _CONST_GPU_ADJ(pos,j) >= len) //not yet in the permutation
              continue;            
        
        sum += minla_gpu_Abs(tag_pos - tag[ _CONST_GPU_ADJ(pos,j) ]);
            
    }//for
    return sum;

}/////////////////////////////


__global__ void minla_const_gpu_node_explorer(int cutoff_depth, unsigned long long *tree_size, int *qtd_sols, int *best_sols,  
    Minla_node *pool, int pool_size, int upper_bound){

    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    
    if(idx<pool_size){ //idx check

        unsigned flag = 0;
        unsigned bit_test = 0;
        int permutation[_MAX_]; //representa o ciclo
        int i, depth; //para dizer que 0-1 ja foi visitado e a busca comeca de 1, bote 2
     
        unsigned long long local_tree = 0ULL;
        int best_sol = upper_bound;
        
        int num_sols = 0;
        int N = const_N;
        int partial_cost = 0;
        int partial_sol = 0;    
        int stack[_MAX_];

        for (i = 0; i < N; ++i) { //
            permutation[i] = _EMPTY_;
        }

        depth = cutoff_depth;
        flag = pool[idx].flag;
        partial_sol = pool[idx].cost;

        for(i = 0; i<cutoff_depth;++i)
            permutation[i] = pool[idx].permutation[i];

        
        while(true){ //search itself

            permutation[depth]++;
            bit_test = 0;
            bit_test |= (1<<permutation[depth]);

            if(permutation[depth] == N){ //all combinations for a given depth have been evaluated
                permutation[depth] = _EMPTY_;
            }
            else{

                if (!(flag & bit_test) ){ //is it valid?

                    partial_cost = minla_const_gpu_partial_cost(permutation,depth+1);

                    if(partial_sol+partial_cost < best_sol){
                    
                        flag |= (1ULL<<permutation[depth]);
                        partial_sol += partial_cost; 
                        
                        stack[depth] = partial_cost;
                        
                        depth++;
                        ++local_tree;
                        
                        if (depth == N){ //a complete solution 
                            
                            ++num_sols;
                            best_sol = partial_sol;
                            num_sols+=1;

                        }//complete solution
                        else continue;
                    }//prune by value
                    else continue;
                }
                else continue;

            }//first else

            depth--; 
            
            partial_sol-=stack[depth];
            flag &= ~(1ULL<<permutation[depth]);

            if(depth < cutoff_depth)
                break;
            //termination condition of the search

        }//end while -- end of the enumeration

        tree_size[idx] = local_tree;
        qtd_sols[idx]  = num_sols;
        best_sols[idx] = best_sol;

    }//if idx   
 
}

__global__ void minla_gpu_node_explorer(int cutoff_depth, unsigned long long *tree_size, int *qtd_sols, int *best_sols,  
    int *gpu_adj_list, int *gpu_size_adj_list,
    Minla_node *pool, int pool_size, int upper_bound, int numNodes){

    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    
    if(idx<pool_size){ //idx check

        unsigned flag = 0;
        unsigned bit_test = 0;
        int permutation[_MAX_]; //representa o ciclo
        int i, depth; //para dizer que 0-1 ja foi visitado e a busca comeca de 1, bote 2
     
        unsigned long long local_tree = 0ULL;
        int best_sol = upper_bound;
        
        int num_sols = 0;
        int N = numNodes;
        int partial_cost = 0;
        int partial_sol = 0;    
        int stack[_MAX_];
        int stride = N-1;

        for (i = 0; i < N; ++i) { //
            permutation[i] = _EMPTY_;
        }

        depth = cutoff_depth;
        flag = pool[idx].flag;
        partial_sol = pool[idx].cost;

        for(i = 0; i<cutoff_depth;++i)
            permutation[i] = pool[idx].permutation[i];

        
        while(true){ //search itself

            permutation[depth]++;
            bit_test = 0;
            bit_test |= (1<<permutation[depth]);

            if(permutation[depth] == N){ //all combinations for a given depth have been evaluated
                permutation[depth] = _EMPTY_;
            }
            else{

                if (!(flag & bit_test) ){ //is it valid?

                    partial_cost = minla_gpu_partial_cost(permutation,depth+1, gpu_adj_list, gpu_size_adj_list, stride);

                    if(partial_sol+partial_cost < best_sol){
                    
                        flag |= (1ULL<<permutation[depth]);
                        partial_sol += partial_cost; 
                        
                        stack[depth] = partial_cost;
                        
                        depth++;
                        ++local_tree;
                        
                        if (depth == N){ //a complete solution 
                            
                            ++num_sols;
                            best_sol = partial_sol;
                            num_sols+=1;

                        }//complete solution
                        else continue;
                    }//prune by value
                    else continue;
                }
                else continue;

            }//first else

            depth--; 
            
            partial_sol-=stack[depth];
            flag &= ~(1ULL<<permutation[depth]);

            if(depth < cutoff_depth)
                break;
            //termination condition of the search

        }//end while -- end of the enumeration

        tree_size[idx] = local_tree;
        qtd_sols[idx]  = num_sols;
        best_sols[idx] = best_sol;

    }//if idx   
 
}


void minla_call_multigpu_kernel(int gpu_id, int cutoff_depth, unsigned long long *tree_size,
    int *qtd_sols, Grafo *grafo, int upper_bound, int pool_size,  Minla_node *subsolutions_pool_h){
    
    cudaSetDevice(gpu_id);
   // cudaFuncSetCacheConfig(BP_queens_root_dfs,cudaFuncCachePreferL1);
   
    unsigned long long *vector_of_tree_size_d;
    int *qtd_sols_d, *best_sol_d,  *gpu_adj_list_d, *gpu_size_adj_list_d;
    
    Minla_node *subsolutions_pool_d;

    int local_qtd_sols = 0;
    unsigned long long local_tree_size = 0ULL;


    int num_blocks = ceil((double)pool_size/_MINLA_BLOCK_SIZE_);

    /////////////////////
    ////Host buffer
    ////////////////////
    int *qtd_sols_h =  (int *) malloc (sizeof(int)*pool_size);
    int *best_sol_h =  (int *) malloc (sizeof(int)*pool_size);
    unsigned long long *vector_of_tree_size_h = (unsigned long long *) malloc (sizeof(unsigned long long)*pool_size);


    /////////////////////
    ////GPU Buffer
    ////////////////////

    cudaMalloc((void**) &vector_of_tree_size_d, pool_size*sizeof(unsigned long long));
    cudaMalloc((void**) &qtd_sols_d,pool_size*sizeof(int));
    cudaMalloc((void**) &best_sol_d, pool_size*sizeof(int));
    cudaMalloc((void**) &gpu_adj_list_d, grafo->numNodes*(grafo->numNodes-1)*sizeof(int));
    cudaMalloc((void**) &gpu_size_adj_list_d, grafo->numNodes*sizeof(int));
    cudaMalloc((void**) &best_sol_d, pool_size*sizeof(int));
    cudaMalloc((void**) &subsolutions_pool_d,pool_size*sizeof(Minla_node));

    //memcopy 

    cudaMemcpy(subsolutions_pool_d, subsolutions_pool_h, pool_size * sizeof(Minla_node), cudaMemcpyHostToDevice);
    cudaMemcpy(gpu_adj_list_d,  grafo->gpu_adj_list, grafo->numNodes*(grafo->numNodes-1)*sizeof(int) , cudaMemcpyHostToDevice);
    cudaMemcpy(gpu_size_adj_list_d, grafo->gpu_size_adj_list, grafo->numNodes*sizeof(int), cudaMemcpyHostToDevice);


    std::cout<<"### Regular BP-DFS search. ###\n";

    minla_gpu_node_explorer<<< num_blocks,_MINLA_BLOCK_SIZE_>>> (cutoff_depth, vector_of_tree_size_d, qtd_sols_d, best_sol_d,gpu_adj_list_d, 
        gpu_size_adj_list_d, subsolutions_pool_d, pool_size,upper_bound, grafo->numNodes);  

  

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
    *tree_size += local_tree_size;
    *qtd_sols += local_qtd_sols;

    cudaFree(vector_of_tree_size_d);
    cudaFree(qtd_sols_d);
    cudaFree(subsolutions_pool_d);
    //After that, Chapel reduces the values
}



void minla_call_const_multigpu_kernel(int gpu_id, int cutoff_depth, unsigned long long *tree_size,
    int *qtd_sols, Grafo *grafo, int upper_bound, int pool_size,  Minla_node *subsolutions_pool_h){
    
    cudaSetDevice(gpu_id);
   // cudaFuncSetCacheConfig(BP_queens_root_dfs,cudaFuncCachePreferL1);
   
    unsigned long long *vector_of_tree_size_d;
    int *qtd_sols_d, *best_sol_d;
    
    Minla_node *subsolutions_pool_d;

    int local_qtd_sols = 0;
    unsigned long long local_tree_size = 0ULL;
    int stride = (grafo->numNodes)-1;


    int num_blocks = ceil((double)pool_size/_MINLA_BLOCK_SIZE_);

    /////////////////////
    ////Host buffer
    ////////////////////
    int *qtd_sols_h =  (int *) malloc (sizeof(int)*pool_size);
    int *best_sol_h =  (int *) malloc (sizeof(int)*pool_size);
    unsigned long long *vector_of_tree_size_h = (unsigned long long *) malloc (sizeof(unsigned long long)*pool_size);


    /////////////////////
    ////GPU Buffer
    ////////////////////

    cudaMalloc((void**) &vector_of_tree_size_d, pool_size*sizeof(unsigned long long));
    cudaMalloc((void**) &qtd_sols_d,pool_size*sizeof(int));
    cudaMalloc((void**) &best_sol_d, pool_size*sizeof(int));
    cudaMalloc((void**) &subsolutions_pool_d,pool_size*sizeof(Minla_node));

    //memcopy 

    cudaMemcpy(subsolutions_pool_d, subsolutions_pool_h, pool_size * sizeof(Minla_node), cudaMemcpyHostToDevice);
    //cudaMemcpy(gpu_adj_list_d,  grafo->gpu_adj_list, grafo->numNodes*(grafo->numNodes-1)*sizeof(int) , cudaMemcpyHostToDevice);
    //cudaMemcpy(gpu_size_adj_list_d, grafo->gpu_size_adj_list, grafo->numNodes*sizeof(int), cudaMemcpyHostToDevice);

    /// Copy to the constant region
    cudaMemcpyToSymbol(const_gpu_adj_list,  grafo->gpu_adj_list, grafo->numNodes*(grafo->numNodes-1)*sizeof(int));
    cudaMemcpyToSymbol(const_gpu_size_adj_list, grafo->gpu_size_adj_list, grafo->numNodes*sizeof(int));
    cudaMemcpyToSymbol(const_N, &grafo->numNodes, sizeof(int));
    cudaMemcpyToSymbol(const_stride, &stride, sizeof(int));

    std::cout<<"### Const BP-DFS search. ###\n";

   // minla_gpu_node_explorer<<< num_blocks,_MINLA_BLOCK_SIZE_>>> (cutoff_depth, vector_of_tree_size_d, qtd_sols_d, best_sol_d,gpu_adj_list_d, 
   //     gpu_size_adj_list_d, subsolutions_pool_d, pool_size,upper_bound, grafo->numNodes);  


    minla_const_gpu_node_explorer<<< num_blocks,_MINLA_BLOCK_SIZE_>>> (cutoff_depth, vector_of_tree_size_d, qtd_sols_d, best_sol_d,subsolutions_pool_d, 
        pool_size,upper_bound);   
 

    gpuErrchk( cudaDeviceSynchronize() );
    gpuErrchk( cudaPeekAtLastError() );
    std::cout<<"### End of the Const GPU search ###\n";

    cudaMemcpy(vector_of_tree_size_h,vector_of_tree_size_d,pool_size*sizeof(unsigned long long),cudaMemcpyDeviceToHost);
    cudaMemcpy(qtd_sols_h,qtd_sols_d,pool_size*sizeof(int),cudaMemcpyDeviceToHost);

    for(int i = 0; i<pool_size;++i){
        local_tree_size += vector_of_tree_size_h[i];
        local_qtd_sols  += qtd_sols_h[i];
    }

    std::cout<<"Local tree size: "<<local_tree_size<<"\n";
    std::cout<<"qtd_sols: "<<local_qtd_sols<<"\n";
    *tree_size += local_tree_size;
    *qtd_sols += local_qtd_sols;

    cudaFree(vector_of_tree_size_d);
    cudaFree(qtd_sols_d);
    cudaFree(subsolutions_pool_d);
    //After that, Chapel reduces the values
}
