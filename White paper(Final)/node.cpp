#include "node.h"

Node::Node()
{
    numEdges = 0;
}

void Node::push_back(int src)
{

    edges.push_back(src);

    // for(int i = 0; i < edges.size(); i++){
    //     std::cout << "edge " << edges[i] << std::endl;
    // }

    numEdges++;

}
