#include "hash_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// DJB2 Hash Algorithm for strings
static unsigned long hash_string(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash % TABLE_SIZE;
}

C_HashTable* hash_table_create(void) {
    C_HashTable* table = (C_HashTable*)malloc(sizeof(C_HashTable));
    if (!table) return NULL;
    for (int i = 0; i < TABLE_SIZE; i++) {
        table->buckets[i] = NULL;
    }
    table->count = 0;
    return table;
}

bool hash_table_put(C_HashTable* table, const char* token, C_PatientRecord record) {
    if (!table || !token) return false;
    unsigned long idx = hash_string(token);

    // Check if key already exists, if so update
    C_HashEntry* curr = table->buckets[idx];
    while (curr) {
        if (strcmp(curr->key, token) == 0) {
            curr->value = record;
            return true;
        }
        curr = curr->next;
    }

    // Insert at front of bucket chain
    C_HashEntry* new_entry = (C_HashEntry*)malloc(sizeof(C_HashEntry));
    if (!new_entry) return false;

    strncpy(new_entry->key, token, sizeof(new_entry->key) - 1);
    new_entry->key[sizeof(new_entry->key) - 1] = '\0';
    new_entry->value = record;
    new_entry->next = table->buckets[idx];
    table->buckets[idx] = new_entry;
    table->count++;
    return true;
}

bool hash_table_get(const C_HashTable* table, const char* token, C_PatientRecord* out_record) {
    if (!table || !token) return false;
    unsigned long idx = hash_string(token);

    C_HashEntry* curr = table->buckets[idx];
    while (curr) {
        if (strcmp(curr->key, token) == 0) {
            if (out_record) *out_record = curr->value;
            return true;
        }
        curr = curr->next;
    }
    return false;
}

bool hash_table_remove(C_HashTable* table, const char* token) {
    if (!table || !token) return false;
    unsigned long idx = hash_string(token);

    C_HashEntry* curr = table->buckets[idx];
    C_HashEntry* prev = NULL;

    while (curr) {
        if (strcmp(curr->key, token) == 0) {
            if (prev) {
                prev->next = curr->next;
            } else {
                table->buckets[idx] = curr->next;
            }
            free(curr);
            table->count--;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }
    return false;
}

void hash_table_destroy(C_HashTable* table) {
    if (!table) return;
    for (int i = 0; i < TABLE_SIZE; i++) {
        C_HashEntry* curr = table->buckets[i];
        while (curr) {
            C_HashEntry* next = curr->next;
            free(curr);
            curr = next;
        }
    }
    free(table);
}

void hash_table_print(const C_HashTable* table) {
    if (!table || table->count == 0) {
        printf("   [Hash table is empty]\n");
        return;
    }
    printf("   ┌───── Hash Table Bucket Index (Total Entries: %d) ─────┐\n", table->count);
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (table->buckets[i]) {
            printf("   │ Bucket [%2d]: ", i);
            C_HashEntry* curr = table->buckets[i];
            while (curr) {
                printf("[%s: %s (Score %d)] -> ", curr->key, curr->value.name, curr->value.effectiveScore);
                curr = curr->next;
            }
            printf("NULL\n");
        }
    }
    printf("   └────────────────────────────────────────────────────────┘\n");
}
