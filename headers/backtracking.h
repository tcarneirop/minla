#ifndef BT_H
#define BT_H


#include "grafo.h"
#include "full_perm.h"
#include "backtracking.h"
#include <limits>
#include <iostream>

int minla_bt_serial(unsigned long long *tree_size, unsigned long long *qtd_sol, Grafo *grafo, int *permutation, int upper_bound);

#endif