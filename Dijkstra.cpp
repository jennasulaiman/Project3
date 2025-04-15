 #include "Graph.h"
#include <unordered_map>
#include <queue>
#include <limits>

std::vector<Node> reconstructPath(std::unordered_map<Node, Node, NodeHash>& prev, Node goal, Node start) {
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

std::vector<Node> dijkstra(const Graph& graph, const Node& start, const Node& goal) {
    std::unordered_map<Node, double, NodeHash> dist;
    std::unordered_map<Node, Node, NodeHash> prev;
    auto cmp = [&](const Node& a, const Node& b) {
        return dist[a] > dist[b];
    };
    std::priority_queue<Node, std::vector<Node>, decltype(cmp)> pq(cmp);

    dist[start] = 0;
    pq.push(start);

    while (!pq.empty()) {
        Node current = pq.top(); pq.pop();
        if (!graph.adjList.count(current)) continue;
        if (current == goal) break;
        for (const Edge& edge : graph.adjList.at(current)) {
            double alt = dist[current] + edge.weight;
            if (!dist.count(edge.dest) || alt < dist[edge.dest]) {
                dist[edge.dest] = alt;
                prev[edge.dest] = current;
                pq.push(edge.dest);
            }
        }
    }
    return reconstructPath(prev, goal, start);
}
