#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <algorithm>
#include <chrono>
#include "Graph.h"

#pragma comment(lib, "ws2_32.lib")

std::vector<Node> dijkstra(const Graph& graph, const Node& start, const Node& goal);
std::vector<Node> aStar(const Graph& graph, const Node& start, const Node& goal);

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }

    Graph graph;
    graph.loadFromCSV("orlando_graph.csv");
    std::cout << "Graph has " << graph.adjList.size() << " nodes loaded.\n";

    int server_fd, client_fd;
    struct sockaddr_in address;
    int opt = 1;
    int port = 8081;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 1);
    std::cout << "Server listening on port " << port << "...\n";

    socklen_t addrlen = sizeof(address);
    client_fd = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    std::cout << "Connection accepted. Waiting for data...\n";

    char buffer[1024] = {0};
    int valread = recv(client_fd, buffer, 1024, 0);
    std::string input(buffer);
    std::replace(input.begin(), input.end(), '\n', '\0');
    std::cout << "Received: " << input << "\n";

    std::stringstream ss(input);
    std::string coord1, coord2;
    getline(ss, coord1, ';');
    getline(ss, coord2);

    float sx = 0, sy = 0, gx = 0, gy = 0;
    sscanf(coord1.c_str(), "%f,%f", &sx, &sy);
    sscanf(coord2.c_str(), "%f,%f", &gx, &gy);

    std::cout << "Start coords: " << sx << ", " << sy << "\n";
    std::cout << "Goal coords : " << gx << ", " << gy << "\n";

    Node start = graph.findClosestNode(sx, sy);
    Node goal = graph.findClosestNode(gx, gy);

    std::cout << "Closest start node: " << start.x << ", " << start.y << std::endl;
    std::cout << "Closest goal node : " << goal.x << ", " << goal.y << std::endl;

    if ((start.x == 0 && start.y == 0) || (goal.x == 0 && goal.y == 0)) {
        std::cerr << "Failed to find valid start or goal node. Aborting.\n";
        closesocket(client_fd);
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    auto dStart = std::chrono::high_resolution_clock::now();
    std::vector<Node> dijkstraPath = dijkstra(graph, start, goal);
    auto dEnd = std::chrono::high_resolution_clock::now();

    auto aStart = std::chrono::high_resolution_clock::now();
    std::vector<Node> astarPath = aStar(graph, start, goal);
    auto aEnd = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> dTime = dEnd - dStart;
    std::chrono::duration<double, std::milli> aTime = aEnd - aStart;

    std::ostringstream out;
    out << "D:";
    for (const Node& n : dijkstraPath) {
        out << n.x << "," << n.y << ";";
    }
    out << "|A:";
    for (const Node& n : astarPath) {
        out << n.x << "," << n.y << ";";
    }

    std::string msg = out.str() + "\n";
    std::cout << "Sending to Processing: " << msg << "\n";

    std::cout << "Dijkstra steps: " << dijkstraPath.size() << ", Time: " << dTime.count() << " ms\n";
    std::cout << "A* steps: " << astarPath.size() << ", Time: " << aTime.count() << " ms\n";

    send(client_fd, msg.c_str(), msg.length(), 0);

    closesocket(client_fd);
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
