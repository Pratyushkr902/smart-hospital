/**
 * @file queue.h
 * @brief Pure C Circular FIFO Queue for General Walk-in Patients
 * 
 * Demonstrates:
 * - Circular buffer array implementation
 * - Modulo arithmetic: index = (index + 1) % capacity
 * - O(1) enqueue and O(1) dequeue
 */

#ifndef QUEUE_H
#define QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "heap.h"
#include <stdbool.h>

typedef struct {
    C_PatientRecord* items;
    int front;
    int rear;
    int size;
    int capacity;
} C_CircularQueue;

C_CircularQueue* queue_create(int capacity);
bool queue_enqueue(C_CircularQueue* q, C_PatientRecord record);
bool queue_dequeue(C_CircularQueue* q, C_PatientRecord* out_record);
bool queue_peek(const C_CircularQueue* q, C_PatientRecord* out_record);
bool queue_is_empty(const C_CircularQueue* q);
bool queue_is_full(const C_CircularQueue* q);
void queue_destroy(C_CircularQueue* q);
void queue_print(const C_CircularQueue* q);

#ifdef __cplusplus
}
#endif

#endif // QUEUE_H
