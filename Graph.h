 #ifndef GRAPH_H
#define GRAPH_H

#include "Node.h"
#include <unordered_map>
#include <vector>
#include <string>

struct Edge {
    Node dest;
    double weight;
};

class Graph {
public:
    std::unordered_map<Node, std::vector<Edge>, NodeHash> adjList;
    void loadFromCSV(const std::string& filename);
    Node getRandomNode();
    Node findClosestNode(float x, float y) const;
};

#endif // GRAPH_H
