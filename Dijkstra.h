#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "Graph.h"
#include <vector>

std::vector<Node> dijkstra(const Graph& graph, const Node& start, const Node& goal);

#endif
