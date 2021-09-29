#ifndef PARTIAL_SEARCH_H
#define PARTIAL_SEARCH_H

#include <limits>
#include "grafo.h"
#include "minla_node.h"

int minla_partial_search(int cutoff_depth, unsigned long long *tree_size, unsigned long long *qtd_valid_subsolutions, Grafo *grafo, Minla_node *pool, int upper_bound);
int minla_get_num_subsolutions(Grafo *grafo,int cutoff_depth);
Minla_node* minla_start_pool(Grafo *grafo, int cutoff_depth);
void minla_print_pool(Minla_node *pool, int pool_size, int cutoff_depth);
int minla_max_pool_size(Grafo *grafo,int cutoff_depth);
#endif