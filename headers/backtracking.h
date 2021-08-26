#ifndef BT_H
#define BT_H




#include "grafo.h"
#include "full_perm.h"

int BP_backtracking_serial(unsigned long long *tree_size, unsigned long long *qtd_sol, Grafo *grafo, int *permutation, int best_sol);

#endif