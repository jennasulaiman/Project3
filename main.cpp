 #include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include "Graph.h"

std::vector<Node> dijkstra(const Graph& graph, const Node& start, const Node& goal);
std::vector<Node> aStar(const Graph& graph, const Node& start, const Node& goal);

void exportPath(const std::vector<Node>& path, const std::string& filename) {
    std::ofstream out(filename);
    for (auto& node : path) {
        out << node.x << "," << node.y << "\n";
    }
    out.close();
}

int main() {
    srand(time(0));
    Graph graph;
    graph.loadFromCSV("orlando_graph.csv");

    Node start = graph.getRandomNode();
    Node goal = graph.getRandomNode();

    std::cout << "Start: " << start.x << ", " << start.y << std::endl;
    std::cout << "Goal : " << goal.x << ", " << goal.y << std::endl;

    clock_t dStart = clock();
    std::vector<Node> dijkstraPath = dijkstra(graph, start, goal);
    clock_t dEnd = clock();

    clock_t aStart = clock();
    std::vector<Node> astarPath = aStar(graph, start, goal);
    clock_t aEnd = clock();

    if (!dijkstraPath.empty()) {
        std::cout << "Dijkstra Path Length: " << dijkstraPath.size() << std::endl;
        std::cout << "Dijkstra Time: " << 1000.0 * (dEnd - dStart) / CLOCKS_PER_SEC << " ms" << std::endl;
        exportPath(dijkstraPath, "dijkstra_path.csv");
    } else {
        std::cout << "Dijkstra: No path found.\n";
    }

    if (!astarPath.empty()) {
        std::cout << "A* Path Length: " << astarPath.size() << std::endl;
        std::cout << "A* Time: " << 1000.0 * (aEnd - aStart) / CLOCKS_PER_SEC << " ms" << std::endl;
        exportPath(astarPath, "astar_path.csv");
    } else {
        std::cout << "A*: No path found.\n";
    }

    return 0;
}


