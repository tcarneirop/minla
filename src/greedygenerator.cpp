#include "../headers/greedygenerator.h"

bool wayToSort(pair<int , int > a, pair<int , int > b){
    return a.first > b.first;
}

GreedyGenerator::GreedyGenerator(Grafo *grafo):grafo(grafo)
{

    pair<int , int> w;

    for(int i = 0; i < grafo->numNodes; i++){
        w.first = grafo->Neighbors[i].size();
        w.second = i;
        vertice.push_back(w);
    }

    std::sort(vertice.begin(),vertice.end(), [](pair<int , int > a, pair<int , int > b)->bool{return a.first > b.first;});

}


int GreedyGenerator::solve(int output[]){
    std::fill_n(output, grafo->numNodes, -1); //preenche o vetor com -1
    int nextTag = 0;

    for(auto i : vertice){
        if(output[ i.second ] == -1){
            int n = 0; // vizinhos não rotulados de i
            int* list = grafo->Adj[ i.second ];
            for(int j = 0; j < i.first; j++){
                if(output[ list[j] ] == -1){
                    n++;
                }
            }
            int x = nextTag + n/2;
            output[ i.second ] = x;
            for(int j = 0; j < i.first; j++){
                if(output[ list[j] ] == -1){
                    if(nextTag == x){
                        nextTag++;
                    }
                    output[ list[j] ] = nextTag;
                    nextTag++;
                }
            }

        }
    }


    return grafo->cost(output);
}
