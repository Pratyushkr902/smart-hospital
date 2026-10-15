/**
 * @file linked_list.h
 * @brief Pure C Doubly Linked List for patient queue manipulation
 * 
 * Demonstrates:
 * - Pointer manipulation (prev, next)
 * - Dynamic node allocation and deallocation
 * - O(1) insertion at head and tail
 * - O(1) deletion given pointer to node
 * - Bidirectional traversal
 */

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "heap.h"
#include <stdbool.h>

typedef struct C_ListNode {
    C_PatientRecord data;
    struct C_ListNode* prev;
    struct C_ListNode* next;
} C_ListNode;

typedef struct {
    C_ListNode* head;
    C_ListNode* tail;
    int size;
} C_DoublyLinkedList;

C_DoublyLinkedList* list_create(void);
bool list_append(C_DoublyLinkedList* list, C_PatientRecord record);
bool list_prepend(C_DoublyLinkedList* list, C_PatientRecord record);
bool list_remove_by_token(C_DoublyLinkedList* list, const char* token, C_PatientRecord* out_record);
C_ListNode* list_find_by_token(const C_DoublyLinkedList* list, const char* token);
void list_destroy(C_DoublyLinkedList* list);
void list_print(const C_DoublyLinkedList* list);

#ifdef __cplusplus
}
#endif

#endif // LINKED_LIST_H
