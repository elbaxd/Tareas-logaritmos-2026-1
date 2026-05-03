#include "rtree_node.hpp"

#include <limits>

// Documentado en rtree_node.hpp
bool isLeaf(const Node& n) {
    return n.k > 0 && n.entries[0].child_idx == -1;
}

// Documentado en rtree_node.hpp
MBR computeNodeMBR(const Node& n) {
    return computeRangeMBR(n.entries, static_cast<std::size_t>(n.k));
}

// Documentado en rtree_node.hpp
MBR computeRangeMBR(const Entry* entries, std::size_t count) {
    MBR mbr{
        std::numeric_limits<float>::infinity(),
        -std::numeric_limits<float>::infinity(),
        std::numeric_limits<float>::infinity(),
        -std::numeric_limits<float>::infinity()
    };
    for (std::size_t i = 0; i < count; ++i) {
        expand(mbr, entries[i].mbr);
    }
    return mbr;
}
