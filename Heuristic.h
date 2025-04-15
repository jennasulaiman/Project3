 #ifndef HEURISTIC_H
#define HEURISTIC_H

#include "Node.h"
#include <cmath>

inline double heuristic(const Node& a, const Node& b) {
    return std::hypot(a.x - b.x, a.y - b.y);
}

#endif // HEURISTIC_H
