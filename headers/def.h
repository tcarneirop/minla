#ifndef DEF_H
#define DEF_H

#include <iostream>
#include <fstream>
#include <utility> // std::pair
#include <algorithm> //std::for_each
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

//using namespace std;
using namespace boost;

// Definição de um grafo não orientado utilizando Boost Graph Library (BGL).
typedef adjacency_list<vecS, vecS, undirectedS, no_property, no_property> UndirectedGraph;

//Iterator para arestas.
typedef UndirectedGraph::edge_iterator ItEdge;

typedef UndirectedGraph::edge_descriptor Edge;

#endif // DEF_H

template<typename Graph> Graph* load(const char* filename) {
    Graph *ptGraph = 0;
    int nv = 0;
    int ne = 0;
    int u, v;

    std::ifstream input(filename, std::ifstream::in);

    if(input.fail()) {
          std::cerr << " File \"" << filename << "\" not found." << std::endl;
          exit(1);
    }

    std::cout << "Reading file!" << std::endl;
    input >> nv >> ne;

    if(nv < 2){
        std::cerr << " ERROR:minimum number of vertices not accept." << std::endl;
        exit(1);
    }

    ptGraph = new Graph(nv);

    int h = 0;

    while (h < ne) {
        input >> u >> v;
        u--;
        v--;
        std::cout << "Edge " << u << " " << v << std::endl;
        // Adição de uma aresta não orientada ao grafo.
        add_edge(u, v, (*ptGraph));
        h++;

    }

    input.close();

    if (ne != num_edges(*ptGraph)) {
        std::cerr << "Estimated number of edges on file is incorrect" << std::endl;
        exit(1);
    }

    std::cout << "Load file finished!" << std::endl;

    return ptGraph;
}
