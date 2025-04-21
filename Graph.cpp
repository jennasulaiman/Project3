 #include "Graph.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>

void Graph::loadFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // skip header
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string val;
        double x1, y1, x2, y2, weight;
        std::getline(ss, val, ','); x1 = std::stod(val);
        std::getline(ss, val, ','); y1 = std::stod(val);
        std::getline(ss, val, ','); x2 = std::stod(val);
        std::getline(ss, val, ','); y2 = std::stod(val);
        std::getline(ss, val, ','); weight = std::stod(val);

        Node from = {x1, y1};
        Node to = {x2, y2};

        adjList[from].push_back({to, weight});
        adjList[to].push_back({from, weight});
    }
}

Node Graph::getRandomNode() {
    if (adjList.empty()) exit(1);
    auto it = adjList.begin();
    std::advance(it, rand() % adjList.size());
    return it->first;
}

Node Graph::findClosestNode(float x, float y) const {
    Node closest;
    double minDist = 1e9;

    for (const auto& pair : adjList) {
        const Node& node = pair.first;
        double dist = std::hypot(node.x - x, node.y - y);
        if (dist < minDist) {
            minDist = dist;
            closest = node;
        }
    }

    return closest;  // always return closest, no threshold
}
