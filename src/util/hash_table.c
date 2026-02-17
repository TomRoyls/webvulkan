#include <stdlib.h>
#include <string.h>

typedef struct WgvkHashTable {
    void** buckets;
    size_t bucket_count;
    size_t size;
} WgvkHashTable;

static size_t hash_ptr(void* key, size_t bucket_count) {
    return ((size_t)key) % bucket_count;
}

WgvkHashTable* wgvk_hash_table_create(size_t bucket_count) {
    WgvkHashTable* table = malloc(sizeof(WgvkHashTable));
    if (!table) return NULL;
    
    table->buckets = calloc(bucket_count, sizeof(void*));
    if (!table->buckets) {
        free(table);
        return NULL;
    }
    
    table->bucket_count = bucket_count;
    table->size = 0;
    
    return table;
}

void wgvk_hash_table_destroy(WgvkHashTable* table) {
    if (table) {
        free(table->buckets);
        free(table);
    }
}

void wgvk_hash_table_insert(WgvkHashTable* table, void* key, void* value) {
    size_t idx = hash_ptr(key, table->bucket_count);
    table->buckets[idx] = value;
    table->size++;
}

void* wgvk_hash_table_lookup(WgvkHashTable* table, void* key) {
    size_t idx = hash_ptr(key, table->bucket_count);
    return table->buckets[idx];
}

void wgvk_hash_table_remove(WgvkHashTable* table, void* key) {
    size_t idx = hash_ptr(key, table->bucket_count);
    table->buckets[idx] = NULL;
    table->size--;
}
