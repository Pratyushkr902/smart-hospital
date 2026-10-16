#include "avl_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int max(int a, int b) {
    return (a > b) ? a : b;
}

static int height(const C_AVLNode* node) {
    return node ? node->height : 0;
}

static int get_balance(const C_AVLNode* node) {
    return node ? height(node->left) - height(node->right) : 0;
}

static C_AVLNode* create_node(C_PatientRecord record) {
    C_AVLNode* node = (C_AVLNode*)malloc(sizeof(C_AVLNode));
    if (!node) return NULL;
    node->data = record;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

static C_AVLNode* rotate_right(C_AVLNode* y) {
    C_AVLNode* x = y->left;
    C_AVLNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

static C_AVLNode* rotate_left(C_AVLNode* x) {
    C_AVLNode* y = x->right;
    C_AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

static C_AVLNode* insert_node(C_AVLNode* node, C_PatientRecord record, bool* success) {
    if (!node) {
        *success = true;
        return create_node(record);
    }

    int cmp = strcmp(record.token, node->data.token);
    if (cmp < 0) {
        node->left = insert_node(node->left, record, success);
    } else if (cmp > 0) {
        node->right = insert_node(node->right, record, success);
    } else {
        *success = false; // Duplicate token not allowed
        return node;
    }

    node->height = 1 + max(height(node->left), height(node->right));
    int balance = get_balance(node);

    // Left-Left Case
    if (balance > 1 && strcmp(record.token, node->left->data.token) < 0) {
        return rotate_right(node);
    }
    // Right-Right Case
    if (balance < -1 && strcmp(record.token, node->right->data.token) > 0) {
        return rotate_left(node);
    }
    // Left-Right Case
    if (balance > 1 && strcmp(record.token, node->left->data.token) > 0) {
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }
    // Right-Left Case
    if (balance < -1 && strcmp(record.token, node->right->data.token) < 0) {
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }

    return node;
}

C_AVLTree* avl_create(void) {
    C_AVLTree* tree = (C_AVLTree*)malloc(sizeof(C_AVLTree));
    if (!tree) return NULL;
    tree->root = NULL;
    tree->count = 0;
    return tree;
}

bool avl_insert(C_AVLTree* tree, C_PatientRecord record) {
    if (!tree) return false;
    bool success = false;
    tree->root = insert_node(tree->root, record, &success);
    if (success) tree->count++;
    return success;
}

static C_AVLNode* search_node(C_AVLNode* node, const char* token) {
    if (!node) return NULL;
    int cmp = strcmp(token, node->data.token);
    if (cmp == 0) return node;
    if (cmp < 0) return search_node(node->left, token);
    return search_node(node->right, token);
}

bool avl_search_by_token(const C_AVLTree* tree, const char* token, C_PatientRecord* out_record) {
    if (!tree || !token) return false;
    C_AVLNode* node = search_node(tree->root, token);
    if (node) {
        if (out_record) *out_record = node->data;
        return true;
    }
    return false;
}

void avl_inorder_traversal(const C_AVLNode* node) {
    if (!node) return;
    avl_inorder_traversal(node->left);
    printf("   │ Token: %-7s | Name: %-16s | Height: %d │\n",
           node->data.token, node->data.name, node->height);
    avl_inorder_traversal(node->right);
}

static void free_tree_nodes(C_AVLNode* node) {
    if (!node) return;
    free_tree_nodes(node->left);
    free_tree_nodes(node->right);
    free(node);
}

void avl_destroy(C_AVLTree* tree) {
    if (tree) {
        free_tree_nodes(tree->root);
        free(tree);
    }
}

void avl_print(const C_AVLTree* tree) {
    if (!tree || tree->count == 0) {
        printf("   [AVL Tree is empty]\n");
        return;
    }
    printf("   ┌───── AVL Tree In-Order Traversal (Total Nodes: %d) ─────┐\n", tree->count);
    avl_inorder_traversal(tree->root);
    printf("   └─────────────────────────────────────────────────────────┘\n");
}
