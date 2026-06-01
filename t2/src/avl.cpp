#include "avl.hpp"

#include <vector>

Node* AVL::rotateZig(Node* y) {
    Node* x = y->left;
    y->left = x->right;
    x->right = y;
    updateHeight(y);
    updateHeight(x);
    return x;
}

Node* AVL::rotateZag(Node* y) {
    Node* x = y->right;
    y->right = x->left;
    x->left = y;
    updateHeight(y);
    updateHeight(x);
    return x;
}

Node* AVL::rotateLR(Node* z) {
    z->left = rotateZag(z->left);
    return rotateZig(z);
}

Node* AVL::rotateRL(Node* z) {
    z->right = rotateZig(z->right);
    return rotateZag(z);
}

Node* AVL::balance(Node* node) {
    int factor = bf(node);

    if (factor > 1) {
        if (bf(node->left) >= 0)
            return rotateZig(node);
        else
            return rotateLR(node);
    }
    if (factor < -1) {
        if (bf(node->right) <= 0)
            return rotateZag(node);
        else
            return rotateRL(node);
    }

    return node;
}

Node* AVL::insertRec(Node* node, uint32_t key) {
    if (!node) return new Node{key, nullptr, nullptr, 0};

    if (key < node->key)
        node->left = insertRec(node->left, key);
    else if (key > node->key)
        node->right = insertRec(node->right, key);
    else
        return node;

    updateHeight(node);
    return balance(node);
}

void AVL::insert(uint32_t key) {
    root = insertRec(root, key);
}

bool AVL::searchRec(Node* node, uint32_t key) const {
    if (!node) return false;
    if (key < node->key) return searchRec(node->left, key);
    if (key > node->key) return searchRec(node->right, key);
    return true;
}

bool AVL::search(uint32_t key) const {
    return searchRec(root, key);
}

void AVL::clear() {
    clear(root);
    root = nullptr;
}

void AVL::clear(Node* root) {
    if (!root) return;
    std::vector<Node*> stack;
    stack.push_back(root);
    while (!stack.empty()) {
        Node* cur = stack.back();
        stack.pop_back();
        if (cur->left) stack.push_back(cur->left);
        if (cur->right) stack.push_back(cur->right);
        delete cur;
    }
}
