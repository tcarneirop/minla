#include <iostream>
#include <vector>
#include <sys/time.h>
#include <dirent.h>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include "headers/grafo.h"
#include "headers/multistart.h"
#include "headers/hillclimb.h"
#include "headers/full_perm.h"
#include "headers/greedygenerator.h"
#include "headers/greedyadaptativegen.h"
//eu// #include "grasp.h"
//eu// #include "vnd.h"
//eu// #include "vns.h"
//eu// #include "simulatedannealing.h"
//eu// #include "tabusearch.h"
//eu// #include "ils.h"
//eu// #include "gls.h"
//eu// #include "scattersearch.h"
//eu// #include "aed.h"
//eu// #include "ga.h"



double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

double get_cpu_time(){
    return (double) clock() / CLOCKS_PER_SEC;
}

int main(int argc, char *argv[])
{


    DIR *diretorio = 0;
    struct dirent *entrada = 0;
    unsigned char isFile = 0x8;

    diretorio = opendir(argv[1]);

    if (diretorio == 0) {
       std::cerr << "Nao foi possivel abrir diretorio." << std::endl;
       exit (1);
    }

    //vai no diretorio das instancias, uma por uma
    //

    std::ofstream output("resultados/resultados.txt", ofstream::out);

    output << "\t\t\t RELATÓRIO DAS INSTÂNCIAS " << std::endl;

    try {
            while (entrada = readdir(diretorio))
            {
                if(entrada->d_type == isFile)
                {

                    Grafo grafo(argv[1], entrada->d_name); //gera aqui a instancia
                    double cpu0, cpu1;


                    std::cout << "INSTÂNCIA: " << entrada->d_name << endl;

                    output << "INSTÂNCIA: " << entrada->d_name << endl;


                    //this is the permutatiob
                    int* out = new int[grafo.numNodes];
                    int* permutation = new int[grafo.numNodes];

                    int i = {0};
                    std::generate(out, out+grafo.numNodes, [&i]{ return i++; });

                    srand( time( NULL ) );

                    //eu// vector<lsmethod> neighborhoodMethods;

                    //eu// LocalSearch ls(&grafo);

                    //eu// neighborhoodMethods.push_back([&ls](int output[], int bestCost, bool firstImprove) {
                    //eu//     return ls.flip2(output,bestCost,firstImprove,1,10);
                    //eu// });

                    //eu// neighborhoodMethods.push_back([&ls](int output[], int bestCost, bool firstImprove) {
                    //eu//     return ls.flip3(output,bestCost,firstImprove,0,3);
                    //eu// });

                    /// RandomGenerator rg(&grafo);

                   // HillClimb hc(&grafo);

                    //hc.findInitialPoint(out, grafo.numNodes);
                    //int result =  hc.solve(out);

                    //std::cout << std::endl << std::endl << "\n Hill climb: " << result;


                    //MultiStart ms(&grafo, true);
                    
                    //result = ms.solve(1000, out, &vnd, &hc);


                    GreedyGenerator gg(&grafo);

                    GreedyAdaptativeGen gag(&grafo, 1);

                    int result = gag.solve(out);

                    std::cout << std::endl << std::endl << "\n GAG: " << result;
                    std::cout << std::endl << std::endl ;


                    //eu// VND vnd(&grafo, neighborhoodMethods);

                    // GRASP grasp(&grafo,2, true, &vnd); OK!

                    // VNS vns(&grafo, &gag, &vnd); OK!

                    // SimulatedAnnealing sa(&grafo, &gag); OK!

                    // TabuSearch ts(&grafo, &gag, 1000); OK!

                    // ILS ils(&grafo, &gag, &vnd);

                    // GLS gls(&grafo, &gag, &vnd); OK!

                    // ScatterSearch ss(&grafo, &gag, &vnd); OK!

                    /// AED aed(&grafo, &vnd);

                    //eu// GA ga(&grafo, 1000);



               

                    /// result = gg.solve(out);


                    /// result = aed.solve(out,100, 10, 1000, .5);

                    //eu// result = ga.solve(100000, out, .5, &gag);

                    /*
                     * result = ms.solve(1000, out, &vnd, &hc);
                     * result = ils.solve(out, 2, 1000, 1.2);
                     * result = vnd.climb(out, grafo.cost(out));
                     * result = vns.solve(out, 10, 1000);
                     * result = grasp.solve(1000,out);
                     * result = sa.solve(100000, out, 100, 3);
                     * result = ts.solve(1000, out);
                     * result = gls.solve(out, 1000, .1f);
                     * result = ss.solve(out,100, 15, 1000);
                    */


                    // output << "\n CPU Time  = " << cpu1  - cpu0  << " seg" << std::endl;

                    // std::cout << "\n Final Cost:" << result << std::endl;

                    // output << "\n Final Cost:" << result << std::endl;

                    // std::cout << "\n GAP:" << grafo.calculeGap(result) << std::endl;
                    // output << "\n GAP:" << grafo.calculeGap(result) << std::endl;

                    // std::cout << "\n Solution" << std::endl;
                    // output << "\n Solution" << std::endl;
                    // for(int k = 0; k < grafo.numNodes; k++){
                    //   output << " " << out[k];
                    //   std::cout   << " " << out[k];
                    // }
                            
                    // output << "\n\n";
                    cpu0 = get_cpu_time();
                    unsigned long long qtd_sol = 0ULL, tree_size = 0ULL;
                    std::cout <<"\n Full perm:" << std::endl;
                    result = bt_serial(&tree_size, &qtd_sol, &grafo, permutation,grafo.optimal+1);
                    std::cout << std::endl << std::endl << "\n Optimizal solution: " << result;
                    std::cout << "\n Permutation: ";
                    
                    for (auto i = 0; i < grafo.numNodes; ++i)
                        std::cout << permutation[i] << ' ';

                    std::cout<<std::endl<<std::endl<<"Qtd: "<<qtd_sol<<std::endl;
                    std::cout<<std::endl<<"Tree size: "<<tree_size<<std::endl;
                    cpu1 = get_cpu_time();
                    std::cout << std::endl <<"\n CPU Time  = " << cpu1  - cpu0  << " seg" << std::endl;


                    output.close();



                }
            }

        
        }
        catch (string& s) {
            std::cerr << s << std::endl;
            std::cerr << "exception caught" << std::endl;
        }
        catch (...) {
            std::cerr << "unknown exception" << std::endl;
        }

    return 0;
}
