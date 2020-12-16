#ifndef NODE_H
#define NODE_H
#include <vector>
#include <iostream>

class Node
{
public:
    int visits;
    std::vector<int> edges;
public:
    Node();
    void push_back(int v);
};

#endif