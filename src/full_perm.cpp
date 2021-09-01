
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
    int i, nivel; //para dizer que 0-1 ja foi visitado e a busca comeca de 1, bote 2
    unsigned cont  = 0;
    unsigned long long local_tree = 0ULL;
    //int best_sol = std::numeric_limits<int>::max();
    int best_sol = upper_bound;
    
    int current_sol = 0;
    int num_sols = 0;
    //int N = grafo->numNodes+1;
    int N = grafo->numNodes;
    int partial_cost = 0;

    /*init*/
    for (i = 0; i < N; ++i) { //
        vertice[i] = _EMPTY_;
    }


    //vertice[0] = 4;
    //flag |= (1ULL<<4);
    //nivel = 1;

    while(true){ //search itself

        vertice[nivel]++;
        bit_test = 0;
        bit_test |= (1<<vertice[nivel]);

        if(vertice[nivel] == N+1){ //all combinations for a given depth have been evaluated
            vertice[nivel] = _EMPTY_;
        }
        else{

            if (!(flag & bit_test) ){ //is it feasible?

                if(nivel == 0){
                    ++local_tree;
                    flag |= (1ULL<<vertice[nivel]);
                    nivel++;
                    continue;
                } //at least two 
                    
                partial_cost = grafo->cost_test(vertice, nivel+1);

                // std::cout<<"Partial: "<<"\n";
                //  for(int k = 0; k < N; k++){
                //       std::cout   << " " << vertice[k];
                //  }
                // // // //cout<<std::endl<<" Leng: "<< nivel <<" Custo: "<< grafo->cost(vertice)<<" Custo ln: "<<grafo->cost_test(vertice, nivel)<<std::endl;
                // cout<<" Leng: "<< nivel+1<<" Custo: "<< partial_cost<<"\n";
                
                if(partial_cost<best_sol){
                    
                    flag |= (1ULL<<vertice[nivel]);
                    nivel++;
                    ++local_tree;

                    if (nivel == N && partial_cost<best_sol){ //a complete solution 

                        ++num_sols;
                        best_sol = partial_cost;
                        std::copy(vertice, vertice + N, permutation);
                        cout<<std::endl<<"Sol "<<num_sols<< " :"<<best_sol<<" "<<std::endl;
                        
                        for(int k = 0; k < N; k++)
                            std::cout   << " " << vertice[k];
                        std::cout<<"\n\n";
                        //exit(1);
                     
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
        flag &= ~(1ULL<<vertice[nivel]);

        if(nivel < 0)
            break;
        //termination condition of the search


    }//end while -- end of the enumeration


    *tree_size = local_tree;
    *qtd_sol = num_sols;

    return best_sol;
}