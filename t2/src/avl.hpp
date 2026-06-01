#pragma once

#include "node.hpp"
#include <cstdint>

class AVL {
public:
    AVL() : root(nullptr) {}
    ~AVL() { clear(root); }

    void insert(uint32_t key);
    bool search(uint32_t key) const;
    void clear();
    Node* getRoot() const { return root; }

private:
    Node* root;

    Node* insertRec(Node* node, uint32_t key);
    bool searchRec(Node* node, uint32_t key) const;

    static Node* rotateZig(Node* y);
    static Node* rotateZag(Node* y);
    Node* rotateLR(Node* z);
    Node* rotateRL(Node* z);

    Node* balance(Node* node);
    void clear(Node* n);
};
