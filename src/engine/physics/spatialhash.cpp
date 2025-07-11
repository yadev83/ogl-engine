#include "spatialhash.hpp"

namespace Engine::Physics {
    size_t CellHash::operator()(Cell c) const {
        return std::hash<uint64_t>()(((uint64_t)c.x<<32) ^ (uint64_t)c.y);
    }

    bool operator==(const Cell& a, const Cell& b) {
        return a.x == b.x && a.y == b.y;
    }
}