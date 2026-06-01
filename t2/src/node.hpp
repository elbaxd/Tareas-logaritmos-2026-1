#pragma once

#include <cstdint>
#include <algorithm>

struct Node {
    uint32_t key;
    Node* left;
    Node* right;
    int height;
};

inline int getHeight(Node* n) {
    return n ? n->height : -1;
}

inline void updateHeight(Node* n) {
    n->height = std::max(getHeight(n->left), getHeight(n->right)) + 1;
}

inline int bf(Node* n) {
    return getHeight(n->left) - getHeight(n->right);
}
