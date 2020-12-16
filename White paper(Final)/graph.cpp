#include "graph.h"
#include "node.h"

Graph::Graph()
{
}

void Graph::push_back(int src, int dst)
{

  std::unique_ptr<Node> &srcNode = sites[src];
  std::unique_ptr<Node> &dstNode = sites[dst];

  // if key does not exist
  if (srcNode == nullptr)
  {
    srcNode.reset(new Node());

    keys.push_back(src);
  }
  if (dstNode == nullptr)
  {
    dstNode.reset(new Node());
    keys.push_back(dst);
  }

  addEdge(src, dst);
}

// Adds dst to SRC Node and src to DST Node
void Graph::addEdge(int src, int dst)
{
  std::unique_ptr<Node> &srcNode = sites[src];

  if (srcNode != nullptr)
  {
    srcNode->push_back(dst);
  }
}
