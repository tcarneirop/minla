#ifndef MINLA_KERNELS_H
#define MINLA_KERNELS_H

#define _MINLA_BLOCK_SIZE_ 	128

#include "grafo.h"
#include "minla_node.h"
#include "minla_kernels.h"

void minla_call_multigpu_kernel(int gpu_id, int cutoff_depth, unsigned long long *local_tree_size,
	int *qtd_sol, Grafo *grafo, int upper_bound, int pool_size,  Minla_node *subsolutions_pool_h);

#endif