#ifndef PARTIAL_SEARCH_H
#define PARTIAL_SEARCH_H

#include <limits>
#include "grafo.h"

int partial_search(int cutoff_depth, unsigned long long *tree_size, unsigned long long *qtd_valid_subsolutions, Grafo *grafo, int *subsolution_pool, int upper_bound);

#endif