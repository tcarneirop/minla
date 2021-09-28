


int partial_search(int cutoff_depth, unsigned long long *tree_size, unsigned long long *qtd_valid_subsolutions, Grafo *grafo, int *subsolution_pool, int upper_bound){

    unsigned flag = 0;
    unsigned bit_test = 0;
    int vertice[_MAX_]; //representa o ciclo
    int i, nivel; //para dizer que 0-1 ja foi visitado e a busca comeca de 1, bote 2
    unsigned cont  = 0;
    unsigned long long local_tree = 0ULL;
    int best_sol = upper_bound;
    
    int current_sol = 0;
    int num_sols = 0;
    int N = grafo->numNodes;
    int partial_cost = 0;
    int partial_sol = 0;    
    int stack[_MAX_];

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
                    
                partial_cost = grafo->ppartial_cost(vertice,nivel+1);

                if(partial_sol+partial_cost < best_sol){
                

                   // cout<<" Leng: "<< nivel+1<<" Partial cost: "<< current_sol<<" Cost test: "<<partial_sol+grafo->ppartial_cost(vertice,nivel+1)<<"\n";
                
                    flag |= (1ULL<<vertice[nivel]);
                    partial_sol += partial_cost; 
                    
                    stack[nivel] = partial_cost;
                    
                    nivel++;
                    ++local_tree;
                    
                    if (nivel == cutoff_depth ){ //a complete solution 

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