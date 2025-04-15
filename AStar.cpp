  #include "Graph.h"
#include "Heuristic.h"
#include <unordered_map>
#include <queue>
#include <limits>

static std::vector<Node> reconstructPath(std::unordered_map<Node, Node, NodeHash>& prev, Node goal, Node start) {
    std::vector<Node> path;
    if (!prev.count(goal)) return {};
    for (Node at = goal; at != start; at = prev[at]) {
        if (!prev.count(at)) return {};
        path.push_back(at);
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<Node> aStar(const Graph& graph, const Node& start, const Node& goal) {
    std::unordered_map<Node, double, NodeHash> gScore, fScore;
    std::unordered_map<Node, Node, NodeHash> cameFrom;
    auto cmp = [&](const Node& a, const Node& b) {
        return fScore[a] > fScore[b];
    };
    std::priority_queue<Node, std::vector<Node>, decltype(cmp)> openSet(cmp);

    for (const auto& pair : graph.adjList) {
        gScore[pair.first] = std::numeric_limits<double>::infinity();
        fScore[pair.first] = std::numeric_limits<double>::infinity();
    }

    gScore[start] = 0;
    fScore[start] = heuristic(start, goal);
    openSet.push(start);

    while (!openSet.empty()) {
        Node current = openSet.top(); openSet.pop();
        if (current == goal) break;
        if (!graph.adjList.count(current)) continue;
        for (const Edge& edge : graph.adjList.at(current)) {
            double tentativeG = gScore[current] + edge.weight;
            if (tentativeG < gScore[edge.dest]) {
                cameFrom[edge.dest] = current;
                gScore[edge.dest] = tentativeG;
                fScore[edge.dest] = tentativeG + heuristic(edge.dest, goal);
                openSet.push(edge.dest);
            }
        }
    }
    return reconstructPath(cameFrom, goal, start);
}
