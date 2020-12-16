#include "node.h"

Node::Node()
{
    visits = 0;
}

void Node::push_back(int src)
{

    edges.push_back(src);

}
