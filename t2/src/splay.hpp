#pragma once

#include "node.hpp"
#include <cstdint>

class SplayTree {
public:
    SplayTree() : root(nullptr) {}
    ~SplayTree() { clear(root); }

    void insert(uint32_t key);
    bool search(uint32_t key);
    void clear();
    Node* getRoot() const { return root; }

private:
    Node* root;

    static Node* rotateZig(Node* y);
    static Node* rotateZag(Node* y);
    Node* splay(Node* node, uint32_t key);

    void clear(Node* n);
};
