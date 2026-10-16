#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

C_CircularQueue* queue_create(int capacity) {
    if (capacity <= 0) capacity = 16;
    C_CircularQueue* q = (C_CircularQueue*)malloc(sizeof(C_CircularQueue));
    if (!q) return NULL;

    q->items = (C_PatientRecord*)malloc(sizeof(C_PatientRecord) * capacity);
    if (!q->items) {
        free(q);
        return NULL;
    }
    q->front = 0;
    q->rear = -1;
    q->size = 0;
    q->capacity = capacity;
    return q;
}

bool queue_is_empty(const C_CircularQueue* q) {
    return (q == NULL || q->size == 0);
}

bool queue_is_full(const C_CircularQueue* q) {
    return (q != NULL && q->size == q->capacity);
}

bool queue_enqueue(C_CircularQueue* q, C_PatientRecord record) {
    if (!q || queue_is_full(q)) return false;
    q->rear = (q->rear + 1) % q->capacity;
    q->items[q->rear] = record;
    q->size++;
    return true;
}

bool queue_dequeue(C_CircularQueue* q, C_PatientRecord* out_record) {
    if (!q || queue_is_empty(q)) return false;
    if (out_record) {
        *out_record = q->items[q->front];
    }
    q->front = (q->front + 1) % q->capacity;
    q->size--;
    return true;
}

bool queue_peek(const C_CircularQueue* q, C_PatientRecord* out_record) {
    if (!q || queue_is_empty(q)) return false;
    if (out_record) {
        *out_record = q->items[q->front];
    }
    return true;
}

void queue_destroy(C_CircularQueue* q) {
    if (q) {
        if (q->items) free(q->items);
        free(q);
    }
}

void queue_print(const C_CircularQueue* q) {
    if (!q || q->size == 0) {
        printf("   [Circular queue is empty]\n");
        return;
    }
    printf("   ┌───── Circular Queue FIFO Order (Size: %d, Cap: %d) ─────┐\n", q->size, q->capacity);
    for (int i = 0; i < q->size; i++) {
        int idx = (q->front + i) % q->capacity;
        printf("   │ [%d] Token: %-7s | Name: %-15s | Age: %2d │\n",
               i, q->items[idx].token, q->items[idx].name, q->items[idx].age);
    }
    printf("   └────────────────────────────────────────────────────────┘\n");
}
