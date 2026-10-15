#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

C_DoublyLinkedList* list_create(void) {
    C_DoublyLinkedList* list = (C_DoublyLinkedList*)malloc(sizeof(C_DoublyLinkedList));
    if (!list) return NULL;
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

bool list_append(C_DoublyLinkedList* list, C_PatientRecord record) {
    if (!list) return false;
    C_ListNode* node = (C_ListNode*)malloc(sizeof(C_ListNode));
    if (!node) return false;
    node->data = record;
    node->next = NULL;
    node->prev = list->tail;

    if (list->tail) {
        list->tail->next = node;
    } else {
        list->head = node;
    }
    list->tail = node;
    list->size++;
    return true;
}

bool list_prepend(C_DoublyLinkedList* list, C_PatientRecord record) {
    if (!list) return false;
    C_ListNode* node = (C_ListNode*)malloc(sizeof(C_ListNode));
    if (!node) return false;
    node->data = record;
    node->prev = NULL;
    node->next = list->head;

    if (list->head) {
        list->head->prev = node;
    } else {
        list->tail = node;
    }
    list->head = node;
    list->size++;
    return true;
}

C_ListNode* list_find_by_token(const C_DoublyLinkedList* list, const char* token) {
    if (!list || !token) return NULL;
    C_ListNode* curr = list->head;
    while (curr) {
        if (strcmp(curr->data.token, token) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

bool list_remove_by_token(C_DoublyLinkedList* list, const char* token, C_PatientRecord* out_record) {
    C_ListNode* target = list_find_by_token(list, token);
    if (!target) return false;

    if (out_record) {
        *out_record = target->data;
    }

    if (target->prev) {
        target->prev->next = target->next;
    } else {
        list->head = target->next;
    }

    if (target->next) {
        target->next->prev = target->prev;
    } else {
        list->tail = target->prev;
    }

    free(target);
    list->size--;
    return true;
}

void list_destroy(C_DoublyLinkedList* list) {
    if (!list) return;
    C_ListNode* curr = list->head;
    while (curr) {
        C_ListNode* next = curr->next;
        free(curr);
        curr = next;
    }
    free(list);
}

void list_print(const C_DoublyLinkedList* list) {
    if (!list || list->size == 0) {
        printf("   [List is empty]\n");
        return;
    }
    printf("   ┌───── Doubly Linked List Order (Size: %d) ─────┐\n", list->size);
    C_ListNode* curr = list->head;
    int idx = 0;
    while (curr) {
        printf("   │ [%d] Token: %-7s | Name: %-15s | Score: %3d │\n",
               idx++, curr->data.token, curr->data.name, curr->data.effectiveScore);
        curr = curr->next;
    }
    printf("   └───────────────────────────────────────────────┘\n");
}

