#include "../headers/grafo.h"
#include <cstdlib>

Grafo::Grafo(const char* dir, const char* filename)
{
    string file(dir);
    file+=filename;
    int u, v;

    ifstream input(file.c_str(), ifstream::in);

    if(input.fail()) {
          throw("File not found");
    }

    input >> numNodes >> numEdges >> optimal;

    std::cout << "Load file " << filename << " with NumNodes: " << numNodes <<" NumEdges: " << numEdges <<" Optimal: " << optimal << std::endl;


    if(numNodes < 2){
        cerr << " ERROR: minimum number of vertices not accepted." << std::endl;
        exit(1);
    }

    Adj = new int*[numNodes];
    Neighbors = new set<int>[numNodes];

    int h = 0;

    while (h < numEdges) {
        input >> u >> v;
        u--;
        v--;
        std::cout<<"\nu : "<<u<<" v: "<<v<<"\n";
        Neighbors[u].insert(v);
        Neighbors[v].insert(u);

        h++;
    }

    for(int i = 0 ; i < numNodes ; i++){
        Adj[i] = new int[Neighbors[i].size()];
        int count = 0;

        for (int j : Neighbors[i])
            Adj[i][count++] = j;

    }

    input.close();

}

string Grafo::toString(){
    stringstream ss;

    for(int i = 0 ; i < numNodes ; i++){
        ss << i << ":";
        for (int j : Neighbors[i]) {
            ss << j << " ";
        }
        ss << std::endl;
    }

    return ss.str();
}

inline int Abs(int v){

    //int v;           // we want to find the absolute value of v
    unsigned int r;  // the result goes here 
    int const mask = v >> sizeof(int) * CHAR_BIT - 1;

    //return (v ^ mask) - mask;
    return (v + mask) ^ mask;
}


//In short, tag is the permutation
int Grafo::ppartial_cost(int tag[],int len){
    
    int sum = 0;
    int pos = len-1;
    int tag_pos = tag[pos];
    
    int* list = Adj[pos]; //len-1 is the position in the permutation
        
    for(int j = 0 ; j < Neighbors[pos].size(); j++){ //neighborhood of the vertex  

        if( list[j] >= len) //not yet in the permutation
              continue;            
        
        sum += Abs(tag_pos - tag[ list[j] ]);
            
    }//for
    
    return sum;

}/////////////////////////////

int Grafo::cost_test(int tag[], int len){
    int sum = 0;
    for(int i = 0 ; i< len ; i++){
        int* list = Adj[i];
        for(int j = 0 ; j < Neighbors[i].size(); j++){

            if( list[j] >= len) //not yet in the permutation
              {
                //std::cout<<"Out of permutation: node "<<list[j]<<"\n";
                continue;
              }  
            
            if(i < list[j]){/// we can remove it... redundant 
                sum += abs(tag[i] - tag[ list[j] ]);
            }
        }
    }
    return sum;
}

int Grafo::cost(int tag[]){
    int sum = 0;
    for(int i = 0 ; i< numNodes ; i++){
        int* list = Adj[i];
        for(int j = 0 ; j < Neighbors[i].size(); j++){    
            if(i < list[j]){
                sum += abs(tag[i] - tag[ list[j] ]);
            }

        }
    }
    return sum;
}


/// [19:47, 11/08/2021] Mardson: Neighbors são os vizinhos de cada vértice
//  [19:47, 11/08/2021] Mardson: é como se fosse uma lista de adjacência
//  [19:48, 11/08/2021] Mardson: vértices é um arrey com os vértices do grafo. 
//   Quem pode ser todos ou os que mudaram na permutação. Tem que checar a chamada da função para saber

int Grafo::subCost(int tag[], int vertices[], int len){


    for(int k = 0; k < len; k++){
        std::cout   << " " << vertices[k];
    }

    std::cout<<" tag: "<< std::endl;

    for(int k = 0; k < len; k++){
        std::cout << " " << tag[k];
    }



    int cost = 0;
    for(int i = 0; i < len; i++){
        int v = vertices[i];
        int tagv = tag[v];
        int* list = Adj[v];
        int neigLen = Neighbors[v].size();
        for(int x = 0; x < neigLen; x++){
            cost += abs( tag[list[x]] - tagv );
        }
    }


    for(int i = 0; i < len; i++){
        for(int j = i + 1; j < len; j++){
            if(Neighbors[vertices[i]].count(vertices[j]) != 0){
                cost -= abs( tag[vertices[i]] - tag[vertices[j]] );
            }
        }
    }

    return cost; // custo relacionado ao subconjunto de vértices

}

int Grafo::flipDelta(int i, int j, int tag[]){
    int delta = 0;
    int tagi = tag[i];
    int tagj = tag[j];
    int tamj = Neighbors[j].size();
    int tami = Neighbors[i].size();

    int* listI = Adj[i];

    for(int x = 0 ; x < tami; x++)
        delta+= abs(tagj - tag[listI[x]]) - abs(tagi - tag[listI[x]]);

    int* listJ = Adj[j];

    for(int x = 0 ; x < tamj; x++)
        delta+= abs(tagi - tag[listJ[x]]) - abs(tagj - tag[listJ[x]]);

    if(Neighbors[i].count(j) != 0)
        delta+= 2* abs(tagj - tagi);

    return delta;

}

void Grafo::flip(const int i, const int j, int tag[]){
    int aux = tag[i];
    tag[i] = tag[j];
    tag[j] = aux;
}

bool Grafo::IsPermutation(Solution s) {
    std::bitset<TAM> mapa;

    for (int i = 0; i < s.size; i++)
        mapa[s.tag[i]] = 1;

    for (int i = 0; i < s.size; i++)
        if (mapa[i] == 0)
            return false;

    return true;
}

double Grafo::calculeGap(int solValue){
    return (abs(solValue-optimal)/(1e-10+ abs(solValue)))*100;
}
