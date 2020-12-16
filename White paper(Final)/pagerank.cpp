#include <time.h>
#include <omp.h>
#include <fstream>
#include <sstream>
#include <string>
#include "graph.h"
#include <iostream>
#include <stdlib.h>
typedef struct drand48_data RNGstate;

/*******************************************************
 * GetRandomNum
 * 
 * Description: Generates a random number between 0 and 1.
 * @param: 
 *  rngstate,
 *  int, 
 * 
 * @return: double, the random number
 ********************************************************/
double GetRandomNum(RNGstate *state)
{
    double x;
    drand48_r(state, &x);
    // this will reduce the number of private variables
    return x;
}

int main(int argc, char *argv[])
{
    int K = 0;
    double D = 0;
    // loop {number of iterations} [number of threads]
    if (argc < 4)
    {
        printf("Usage: pagerank estimator {K: length of random walk} {D: damping ratio, 0-100} {file: list of sites}\n");
        exit(1);
    }
    K = atoi(argv[1]);
    D = atof(argv[2]);
    std::string testInputFile = argv[3];
    std::cout << "K: " << K << "\n"
              << "D: " << D << "\n"
              << "Input File: " << testInputFile << std::endl;
    double damping = D / 100.0;

    RNGstate state;
    std::ifstream infile(testInputFile);
    time_t ctime = time(NULL);

    Graph mySites;
    int src, dst;
    double coinflip;
    std::cout << testInputFile << std::endl;

    if (!infile)
        std::cerr << "Could not open the file!" << std::endl;

    while (infile >> src >> dst)
    {
        mySites.push_back(src, dst);
    }
    std::cout << mySites.numVertices << std::endl;
    std::cout << mySites.keys.size() << std::endl;
    std::cout << mySites.sites.size() << std::endl;

#pragma omp parallel private(coinflip, state)
    {
        srand48_r(ctime + omp_get_thread_num(), &state);

        coinflip = GetRandomNum(&state);
        // Walk length
        for (int i = 0; i < K; i++)
        {
        }
        if (coinflip < damping)
        {
        }
        else
        {
        }
    }
}
