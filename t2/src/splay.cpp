#include "splay.hpp"

#include <vector>

Node* SplayTree::rotateZig(Node* y) {
    Node* x = y->left;
    y->left = x->right;
    x->right = y;
    return x;
}

Node* SplayTree::rotateZag(Node* y) {
    Node* x = y->right;
    y->right = x->left;
    x->left = y;
    return x;
}

Node* SplayTree::splay(Node* t, uint32_t key) {
    if (!t) return nullptr;

    Node header;
    header.left = nullptr;
    header.right = nullptr;
    Node* leftMax = &header;
    Node* rightMin = &header;

    while (true) {
        if (key < t->key) {
            if (!t->left) break;
            if (key < t->left->key) {
                t = rotateZig(t);
                if (!t->left) break;
            }
            rightMin->left = t;
            rightMin = t;
            t = t->left;
        } else if (key > t->key) {
            if (!t->right) break;
            if (key > t->right->key) {
                t = rotateZag(t);
                if (!t->right) break;
            }
            leftMax->right = t;
            leftMax = t;
            t = t->right;
        } else {
            break;
        }
    }

    rightMin->left = t->right;
    leftMax->right = t->left;
    t->left = header.right;
    t->right = header.left;

    return t;
}

void SplayTree::insert(uint32_t key) {
    if (!root) {
        root = new Node{key, nullptr, nullptr, 0};
        return;
    }

    Node* cur = root;
    Node* parent = nullptr;

    while (cur) {
        parent = cur;
        if (key < cur->key)
            cur = cur->left;
        else if (key > cur->key)
            cur = cur->right;
        else
            return;
    }

    Node* newNode = new Node{key, nullptr, nullptr, 0};
    if (key < parent->key)
        parent->left = newNode;
    else
        parent->right = newNode;

    root = splay(root, key);
}

bool SplayTree::search(uint32_t key) {
    root = splay(root, key);
    return root && root->key == key;
}

void SplayTree::clear() {
    clear(root);
    root = nullptr;
}

void SplayTree::clear(Node* root) {
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
