#include "../headers/greedyadaptativegen.h"

GreedyAdaptativeGen::GreedyAdaptativeGen(Grafo *grafo, int tolerancia):grafo(grafo)
{
    this->tol = tolerancia;
    pair<int , int> w;

    aux = new int[grafo->numNodes];

    for(int i = 0; i < grafo->numNodes; i++){
        w.first = grafo->Neighbors[i].size();
        w.second = i;
        vertice.push_back(w);
    }

    std::sort(vertice.begin(),vertice.end(), [](pair<int , int > a, pair<int , int > b)->bool{return a.first > b.first;});


}

int GreedyAdaptativeGen::solve(int output[]){
    //cout << "GreedyAdaptativeGen Started" << endl;

    std::fill_n(output, grafo->numNodes, -1); //preenche o vetor com -1

    for(int v = 0; v < vertice.size(); v++) aux[v] = vertice[v].second; //copia para o vetor auxiliar

    srand(time(NULL)); //inicializa a semente aleatória

    int nextTag = 0;
    for(int z = 0; z < grafo->numNodes; z++){
        if( output[aux[z]] == -1 ){ // se o z-ésimo maior vértice (grau) ainda não tem rótulo.
            int f = z + 1; //
            int kLen = grafo->Neighbors[aux[z]].size();  // tamanho de adj[aux[z]]

            /// Procure dentre os vértices não rotulados com vizinhanças menores que a minha e ordenados
            /// decrescentemente por vizinhança, o primeiro dentre eles cuja diferença entre a minha viz. e a
            /// dele seja maior que tol.
            while(f < grafo->numNodes && kLen - grafo->Neighbors[aux[f]].size() < tol ) f++;

            int intermediario = z + rand() % (f - z); // gera um número inteiro aleatório entre z e f-1; Talvez o que ele queria seria [z+1, f-1]

            swap(aux[intermediario], aux[z]);

            int vi = aux[z]; // vértice intermediário entre z e f.

            if( output[vi] == -1 ){
                int n = 0; //vizinhos não rotulados de i
                int* list = grafo->Adj[vi];
                int tam = grafo->Neighbors[vi].size();

                /// n == di - 2*( num de vertices rotulados)
                for(int j = 0; j < tam; j++){ // para cada vizinho de vi.
                    if(output[list[j]] == -1){
                        n++;
                    }else{
                        n--;
                    }
                }
                /// A vizinhança fechada de vi recebe rótulos contíguos, e vi recebe o rótulo médio entre eles.
                int x = nextTag + std::max(0,n) / 2; //escolhe um rótulo central
                output[vi] = x;

                random_shuffle(list, list + tam); //permuta os vizinhos de vi

                for(int j = 0; j < tam; j++){
                    if (output[list[j]] == -1){
                        if(nextTag == x){
                            nextTag++;
                            break;
                        }
                        output[list[j]] = nextTag; //atribui o rótulo atual ao vértice j adjacente a i
                        nextTag++;
                    }
                }
                if (nextTag == x) {
                    nextTag++;
                }

            }
        }
    }

  //  cout << "GreedyAdaptativeGen Done" << endl;

    return grafo->cost(output);

}
