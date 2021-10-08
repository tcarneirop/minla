#ifndef BP_ALL_PERM_H
#define BP_ALL_PERM_H

#define _EMPTY_ -1
#define _MAX_ 64


#include "grafo.h"



int BP_all_perm_serial(unsigned long long *tree_size, unsigned long long *qtd_sol, Grafo *gr, int *permutation);

int NEW_BP_all_perm_serial(unsigned long long *tree_size, unsigned long long *qtd_sol, Grafo *grafo, int *permutation);


int bt_serial(unsigned long long *tree_size, unsigned long long *qtd_sol, Grafo *grafo, int *permutation, int upper_bound);


#endif