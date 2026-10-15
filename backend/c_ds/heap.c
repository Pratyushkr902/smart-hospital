#include "heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void swap(C_PatientRecord* a, C_PatientRecord* b) {
    C_PatientRecord temp = *a;
    *a = *b;
    *b = temp;
}

static void heapify_up(C_MaxHeap* heap, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (heap->data[index].effectiveScore > heap->data[parent].effectiveScore) {
            swap(&heap->data[index], &heap->data[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

static void heapify_down(C_MaxHeap* heap, int index) {
    int size = heap->size;
    while (1) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int largest = index;

        if (left < size && heap->data[left].effectiveScore > heap->data[largest].effectiveScore) {
            largest = left;
        }
        if (right < size && heap->data[right].effectiveScore > heap->data[largest].effectiveScore) {
            largest = right;
        }

        if (largest != index) {
            swap(&heap->data[index], &heap->data[largest]);
            index = largest;
        } else {
            break;
        }
    }
}

C_MaxHeap* heap_create(int initial_capacity) {
    if (initial_capacity <= 0) initial_capacity = 8;
    C_MaxHeap* heap = (C_MaxHeap*)malloc(sizeof(C_MaxHeap));
    if (!heap) return NULL;

    heap->data = (C_PatientRecord*)malloc(sizeof(C_PatientRecord) * initial_capacity);
    if (!heap->data) {
        free(heap);
        return NULL;
    }
    heap->size = 0;
    heap->capacity = initial_capacity;
    return heap;
}

bool heap_insert(C_MaxHeap* heap, C_PatientRecord record) {
    if (!heap) return false;

    // Dynamic resize if capacity is reached (amortized O(1) resizing)
    if (heap->size >= heap->capacity) {
        int new_capacity = heap->capacity * 2;
        C_PatientRecord* new_data = (C_PatientRecord*)realloc(heap->data, sizeof(C_PatientRecord) * new_capacity);
        if (!new_data) return false;
        heap->data = new_data;
        heap->capacity = new_capacity;
    }

    heap->data[heap->size] = record;
    heapify_up(heap, heap->size);
    heap->size++;
    return true;
}

bool heap_extract_max(C_MaxHeap* heap, C_PatientRecord* out_record) {
    if (!heap || heap->size == 0) return false;

    if (out_record) {
        *out_record = heap->data[0];
    }

    heap->data[0] = heap->data[heap->size - 1];
    heap->size--;

    if (heap->size > 0) {
        heapify_down(heap, 0);
    }
    return true;
}

bool heap_peek(const C_MaxHeap* heap, C_PatientRecord* out_record) {
    if (!heap || heap->size == 0) return false;
    if (out_record) {
        *out_record = heap->data[0];
    }
    return true;
}

void heap_destroy(C_MaxHeap* heap) {
    if (heap) {
        if (heap->data) free(heap->data);
        free(heap);
    }
}

void heap_print(const C_MaxHeap* heap) {
    if (!heap || heap->size == 0) {
        printf("   [Heap is empty]\n");
        return;
    }
    printf("   ┌───── Max-Heap Priority State (Size: %d, Cap: %d) ─────┐\n", heap->size, heap->capacity);
    for (int i = 0; i < heap->size; i++) {
        printf("   │ [%d] Score: %3d | Token: %-7s | Name: %-15s │\n",
               i, heap->data[i].effectiveScore, heap->data[i].token, heap->data[i].name);
    }
    printf("   └────────────────────────────────────────────────────────┘\n");
}

