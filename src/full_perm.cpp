
#include <limits>
#include "../headers/grafo.h"
#include "../headers/full_perm.h"
#include <iostream>

int BP_all_perm_serial(unsigned long long *tree_size, unsigned long long *qtd_sol, Grafo *grafo, int *permutation){

    unsigned flag = 0;
    unsigned bit_test = 0;
    int vertice[_MAX_]; //representa o ciclo
    int i, nivel; //para dizer que 0-1 ja foi visitado e a busca comeca de 1, bote 2
    unsigned cont  = 0;
    unsigned long long local_tree = 0ULL;
    int best_sol = std::numeric_limits<int>::max();
    int current_sol;
    int num_sols = 0;
    int N = grafo->numNodes+1;
    int vetex_lb;
   
    /*init*/
    for (i = 0; i < N; ++i) { //
        vertice[i] = -1;
    }
   
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
                    current_sol = grafo->cost((vertice+1));
                    // cout<<std::endl<<"Sol "<<num_sols<< " :"<<current_sol<<" "<<std::endl;
                    //     for(int k = 1; k < N; k++){
                    //         std::cout   << " " << vertice[k];
                    // }
                   if(current_sol<best_sol){
                        best_sol = current_sol;
                        std::copy(vertice+1, vertice + N, permutation);
                        cout<<std::endl<<"Sol "<<num_sols<< " :"<<best_sol<<" "<<std::endl;
                        for(int k = 1; k < N; k++){
                            std::cout   << " " << vertice[k];
                        }
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



int NEW_BP_all_perm_serial(unsigned long long *tree_size, unsigned long long *qtd_sol, Grafo *grafo, int *permutation){

    unsigned flag = 0;
    unsigned bit_test = 0;
    int vertice[_MAX_]; //representa o ciclo
    int i, nivel; //para dizer que 0-1 ja foi visitado e a busca comeca de 1, bote 2
    unsigned cont  = 0;
    unsigned long long local_tree = 0ULL;
    int best_sol = std::numeric_limits<int>::max();
    int current_sol = 0;
    int num_sols = 0;
    int N = grafo->numNodes+1;
   
    /*init*/
    for (i = 0; i < N; ++i) { //
        vertice[i] = -1;
        //maybe we need here 0 instead of -1?
    }
    /*
        to fiz 0-1
     */
    vertice[0] = 0;
    flag |= (1ULL<<0);
    nivel = 1;

    while(true){ //search itself

        vertice[nivel]++;
        bit_test = 0;
        bit_test |= (1<<vertice[nivel]);

        if(vertice[nivel] == N){ //all combinations for a given depth have been evaluated
            vertice[nivel] = _EMPTY_;
        }
        else{

            if (!(flag & bit_test) ){ //is it feasible?

                flag |= (1ULL<<vertice[nivel]);
                nivel++;
                ++local_tree;

                if (nivel == N){ //a complete solution
                     ++num_sols;
                    current_sol = grafo->cost((vertice+1));

                    if(current_sol<best_sol){ //Improves the best sol?
                        best_sol = current_sol;
                        std::copy(vertice+1, vertice + N, permutation);
                        cout<<std::endl<<"Sol "<<num_sols<< " :"<<best_sol<<" "<<std::endl;
                        for(int k = 1; k < N; k++){
                            std::cout   << " " << vertice[k];
                        }
                    }//if best sol

                }//a complete solution
                else
                    continue;
            }
            else //no, not feasible
                continue;

        }//first else

        nivel--; 
        flag &= ~(1ULL<<vertice[nivel]);

        if(nivel <= 0)
            break;
        //termination condition of the search


    }//end while -- end of the enumeration


    *tree_size = local_tree;
    *qtd_sol = num_sols;

    return best_sol;
}


int bt_serial(unsigned long long *tree_size, unsigned long long *qtd_sol, Grafo *grafo, int *permutation, int upper_bound){

    unsigned flag = 0;
    unsigned bit_test = 0;
    int vertice[_MAX_]; //representa o ciclo
    int i, nivel = 0; //para dizer que 0-1 ja foi visitado e a busca comeca de 1, bote 2
    unsigned cont  = 0;
    unsigned long long local_tree = 0ULL;
    int best_sol = upper_bound;
    
    int current_sol = 0;
    int num_sols = 0;
    int N = grafo->numNodes;
    int partial_cost = 0;
    int partial_sol = 0;    
    int stack[_MAX_];
    int vertex_lb;

    /*init*/
    for (i = 0; i < N; ++i) { //
        vertice[i] = _EMPTY_;
    }


    while(true){ //search itself

        vertice[nivel]++;
        bit_test = 0;
        bit_test |= (1<<vertice[nivel]);

        if(vertice[nivel] == N){ //all combinations for a given depth have been evaluated
            vertice[nivel] = _EMPTY_;
        }
        else{

            if (!(flag & bit_test) ){ //is it feasible?

                if(nivel == 0){
                    ++local_tree;
                    stack[nivel] = 0;
                    flag |= (1ULL<<vertice[nivel]);
                    nivel++;
                    continue;
                } //at least two 
                
               // vertex_lb = grafo->get_lower_bound_vertex(vertice[nivel]) + partial_sol;
                partial_cost = grafo->ppartial_cost(vertice,nivel+1);
                
                //std::cout<<"partial cost: "<<partial_sol+partial_cost<<" vertex lb: "<<vertex_lb<<"\n";
                // (vertex_lb < best_sol) && 
                if((partial_sol+partial_cost < best_sol) ){
                
                   // cout<<" Leng: "<< nivel+1<<" Partial cost: "<< current_sol<<" Cost test: "<<partial_sol+grafo->ppartial_cost(vertice,nivel+1)<<"\n";
                
                    flag |= (1ULL<<vertice[nivel]);
                    partial_sol += partial_cost; 
                    
                    stack[nivel] = partial_cost;
                    
                    nivel++;
                    ++local_tree;
                    
                    if (nivel == N ){ //a complete solution 
                        ++num_sols;
                        best_sol = partial_sol;
                        std::copy(vertice, vertice + N, permutation);
                        cout<<std::endl<<"Sol "<<num_sols<< " :"<<best_sol<<" "<<std::endl;
                                     
                    }//a complete solution
                    else//not a complete solution
                        continue;
                }//prune by value
                else
                    continue;
            }
            else //no, not valid
                continue;

        }//first else

        nivel--; 
        //std::cout<<"\n alor";

        partial_sol-=stack[nivel];
        flag &= ~(1ULL<<vertice[nivel]);

        if(nivel < 0)
            break;
        //termination condition of the search


    }//end while -- end of the enumeration


    *tree_size = local_tree;
    *qtd_sol = num_sols;

    return best_sol;
}