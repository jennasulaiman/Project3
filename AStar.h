  #ifndef ASTAR_H
#define ASTAR_H

#include "Graph.h"
#include <vector>

std::vector<Node> aStar(const Graph& graph, const Node& start, const Node& goal);

#endif
