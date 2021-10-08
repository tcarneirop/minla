#ifndef CALL_GPU_SEARCH_H
#define CALL_GPU_SEARCH_H

#include <limits>
#include "grafo.h"
#include "minla_node.h"

#define _MINLA_BLOCK_SIZE_ 128

void minla_call_gpu_search(int, Grafo *, int, std::string);


//int minla_call_multigpu_search(int gpu_id, int cutoff_depth, unsigned long long *local_tree_size, int *best_sol, 
//	int *qtd_sol, Grafo *grafo, int upper_bound, int pool_size,  Minla_node *subsolutions_pool_h);
 
#endif

