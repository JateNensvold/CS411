#include <time.h>
#include <omp.h>
#include <fstream>
#include <sstream>
#include <string>
#include "graph.h"
#include <iostream>
#include <stdlib.h>
#include <queue>
#include <utility>
#include <random>

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

int intRand(const int & min, const int & max) {
    static thread_local std::mt19937 generator;
    std::uniform_int_distribution<int> distribution(min,max);
    return distribution(generator);
}

class comparePair
{
public:
    // Compare two pairs by their second value
    bool operator()(std::pair<int, int> n1, std::pair<int, int> n2)
    {
        return n1.second > n2.second;
    }
};

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

    if (!infile)
        std::cerr << "Could not open the file!" << std::endl;

    while (infile >> src >> dst)
    {
        mySites.push_back(src, dst);
    }
    std::cout << mySites.keys.size() << std::endl;
    std::cout << mySites.sites.size() << std::endl;

#pragma omp parallel for private(coinflip, state)
    for (int node_num = 0; node_num < mySites.keys.size(); node_num++)
    {
        srand48_r(ctime + omp_get_thread_num(), &state);
        int startNode = mySites.keys[node_num];
        coinflip = GetRandomNum(&state);

        int workingNode = startNode;
        // Register a visit to the first node that is started at
        mySites.sites[workingNode]->visits++;
        // Walk length
        int randomNum, nodeSize, mySiteSize;
        for (int distance = 0; distance < K; distance++)
        {
            randomNum = intRand(0, mySites.keys.size());
            // Heads/ random neighbor
            if (coinflip < damping)
            {
                nodeSize = mySites.sites[workingNode]->edges.size();
                workingNode = mySites.sites[workingNode]->edges[randomNum % nodeSize];
            }
            // Tails/ random node
            else
            {
                mySiteSize = mySites.keys.size();
                workingNode = mySites.keys[randomNum % mySiteSize];
            }
            // Register a visit to the new Node
            mySites.sites[workingNode]->visits += 1;
        }
    }

    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, comparePair> pq;
    for (int i = 0; i < 5; i++)
    {
        pq.push(std::make_pair(-1, -1));
    }
    int tempKey, counter = 0;
    for (int i = 0; i < mySites.keys.size(); i++)
    {
        tempKey = mySites.keys[i];
        pq.push(std::make_pair(tempKey, mySites.sites[tempKey]->visits));
        pq.pop();
    }
    std::pair<int, int> tempPair;
    for (int i = 0; i < 5; i++)
    {
        tempPair = pq.top();
        std::cout << "Site: " << tempPair.first << " visits:" << tempPair.second << std::endl;
        pq.pop();
    }
}
