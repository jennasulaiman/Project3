cmake_minimum_required(VERSION 3.30)
project(untitled4)

set(CMAKE_CXX_STANDARD 20)

add_executable(untitled4 main.cpp
        Graph.h
        Graph.cpp
        Dijkstra.cpp
        Dijkstra.h
        AStar.cpp
        AStar.h
        Heuristic.cpp
        Heuristic.h
        Node.h)
