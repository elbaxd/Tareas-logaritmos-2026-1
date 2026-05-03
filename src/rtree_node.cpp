#include "rtree_node.hpp"

#include <limits>

bool isLeaf(const Node& n) {
    return n.k > 0 && n.entries[0].child_idx == -1;
}

MBR computeNodeMBR(const Node& n) {
    return computeRangeMBR(n.entries, static_cast<std::size_t>(n.k));
}

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
