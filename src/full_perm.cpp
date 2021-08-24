
#include <limits>
#include "../headers/grafo.h"
#include "../headers/full_perm.h"
#include <iostream>

int BP_all_perm_serial(unsigned long long *tree_size, unsigned long long *qtd_sol, Grafo *grafo){

    unsigned flag = 0;
    unsigned bit_test = 0;
    unsigned vertice[_MAX_]; //representa o ciclo
    unsigned i, nivel; //para dizer que 0-1 ja foi visitado e a busca comeca de 1, bote 2
    unsigned cont  = 0;
    unsigned long long local_tree = 0ULL;
    int best_sol = std::numeric_limits<int>::max();
    int current_sol;
    int num_sols = 0;
    int N = grafo->numNodes+1;
   
    /*init*/
    for (i = 0; i < N; ++i) { //
        vertice[i] = -1;
        //maybe we need here 0 instead of -1?
    }
    /*
        para dizer que 0-1 sao fixos
     */
    vertice[0] = 0;
    flag |= (1ULL<<0);
    nivel = 1;


    do{

        vertice[nivel]++;
        bit_test = 0;
        bit_test |= (1<<vertice[nivel]);
        //edge =  mat_h(vertice[anterior(nivel)],vertice[nivel]);
        //let's calculate at the end

        if(vertice[nivel] == N){
            
            vertice[nivel] = _EMPTY_;

        }else if (!(flag & bit_test) ){

                //custo +=  edge;
                flag |= (1ULL<<vertice[nivel]);
                nivel++;
                ++local_tree;

                if (nivel == N){ //sol
                    ++num_sols;
                    current_sol = grafo->cost((int*)(vertice+1));
                    if(current_sol<best_sol){
                        best_sol = current_sol;
                        //cout<<std::endl<<"Sol "<<num_sols<<;
                        //for(int k = 1; k < N; k++){
                        //    std::cout   << " " << vertice[k];
                        //}
                        //exit(1);
                    }

                    //CALCULATE THE SOLUTION HERE
            }else continue;
        }else continue;

        nivel--;
        //we dont need, as I intent to build full permutations
       // custo -=  mat_h(vertice[anterior(nivel)],vertice[nivel]);   
        flag &= ~(1ULL<<vertice[nivel]);

    }while(nivel > 0); 

    *tree_size = local_tree;
    *qtd_sol = num_sols;

    return best_sol;
}
