#ifndef OMP_SEARCH_H
#define OMP_SEARCH_H

#include <limits>
#include "grafo.h"
#include "minla_node.h"


void minla_call_omp_search(int cutoff_depth, Grafo *grafo, int upper_bound);

int minla_omp_node_explorer(int cutoff_depth, unsigned long long *tree_size, int *qtd_sols, Grafo *grafo, Minla_node *pool, int node_id, int upper_bound);

#endif
