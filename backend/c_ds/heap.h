/**
 * @file heap.h
 * @brief Pure C implementation of a Max-Heap (Priority Queue)
 * 
 * Demonstrates:
 * - Dynamic memory management (malloc, realloc, free)
 * - Array-based binary tree representation:
 *     Parent(i) = (i - 1) / 2
 *     Left(i)   = 2 * i + 1
 *     Right(i)  = 2 * i + 2
 * - Time Complexity: O(log N) insert, O(log N) extractMax, O(1) peek
 */

#ifndef HEAP_H
#define HEAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    char id[16];
    char token[16];
    char name[64];
    int age;
    int urgencyScore;
    int waitMinutes;
    int effectiveScore; // urgencyScore + waitBonus
} C_PatientRecord;

typedef struct {
    C_PatientRecord* data;
    int size;
    int capacity;
} C_MaxHeap;

/**
 * Initializes a new Max-Heap with given initial capacity
 */
C_MaxHeap* heap_create(int initial_capacity);

/**
 * Inserts a patient record into the heap maintaining the Max-Heap property. O(log N)
 */
bool heap_insert(C_MaxHeap* heap, C_PatientRecord record);

/**
 * Extracts and returns the patient with the highest priority score. O(log N)
 */
bool heap_extract_max(C_MaxHeap* heap, C_PatientRecord* out_record);

/**
 * Peeks at the highest priority patient without removing it. O(1)
 */
bool heap_peek(const C_MaxHeap* heap, C_PatientRecord* out_record);

/**
 * Frees all dynamically allocated memory for the heap
 */
void heap_destroy(C_MaxHeap* heap);

/**
 * Prints the internal array state of the heap
 */
void heap_print(const C_MaxHeap* heap);

#ifdef __cplusplus
}
#endif

#endif // HEAP_H

