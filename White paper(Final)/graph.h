#ifndef GRAPH_H
#define GRAPH_H

#include "node.h"
#include <unordered_map>
#include <map>
#include <memory>
#include <fstream>
#include <iostream>

class Graph
{
public:
    std::unordered_map<int, std::unique_ptr<Node>> sites;
    std::vector<int> keys;

public:
    Graph();
    void push_back(int src, int dst);
    void addEdge(int src, int dst);
};

#endif