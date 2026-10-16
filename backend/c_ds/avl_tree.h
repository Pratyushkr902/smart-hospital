/**
 * @file avl_tree.h
 * @brief Pure C Self-Balancing AVL Binary Search Tree for Patient Indexing
 * 
 * Demonstrates:
 * - Height-balanced BST property: |height(left) - height(right)| <= 1
 * - Tree Rotations: Single (Left/Right) and Double (Left-Right / Right-Left)
 * - Guaranteed O(log N) search, insertion, and traversal
 */

#ifndef AVL_TREE_H
#define AVL_TREE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "heap.h"
#include <stdbool.h>

typedef struct C_AVLNode {
    C_PatientRecord data;
    struct C_AVLNode* left;
    struct C_AVLNode* right;
    int height;
} C_AVLNode;

typedef struct {
    C_AVLNode* root;
    int count;
} C_AVLTree;

C_AVLTree* avl_create(void);
bool avl_insert(C_AVLTree* tree, C_PatientRecord record);
bool avl_search_by_token(const C_AVLTree* tree, const char* token, C_PatientRecord* out_record);
void avl_inorder_traversal(const C_AVLNode* node);
void avl_destroy(C_AVLTree* tree);
void avl_print(const C_AVLTree* tree);

#ifdef __cplusplus
}
#endif

#endif // AVL_TREE_H
