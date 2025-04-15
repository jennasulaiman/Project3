 #ifndef NODE_H
#define NODE_H

#include <cstddef>
#include <functional>

struct Node {
    double x, y;

    bool operator==(const Node& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Node& other) const {
        return !(*this == other);
    }
};

struct NodeHash {
    size_t operator()(const Node& n) const {
        return std::hash<double>()(n.x) ^ std::hash<double>()(n.y);
    }
};

#endif // NODE_H
