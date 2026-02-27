#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Open-addressing hash table with linear probing and tombstone deletion.
 *
 * Keys are pointer-sized values mapped to pointer-sized values.  The table
 * uses two sentinel bucket states:
 *   bucket->key == NULL  → empty (never occupied)
 *   bucket->key == TOMBSTONE → deleted (was occupied, now available for reuse)
 *
 * The load factor is kept below 0.75; the table never shrinks.
 */

#define WGVK_HT_TOMBSTONE ((void*)(uintptr_t)1)
#define WGVK_HT_MAX_LOAD_NUM 3   /* max load = 3/4 */
#define WGVK_HT_MAX_LOAD_DEN 4

typedef struct {
    void* key;
    void* value;
} WgvkHashBucket;

typedef struct WgvkHashTable {
    WgvkHashBucket* buckets;
    size_t bucket_count;
    size_t size;      /* live entries */
    size_t occupied;  /* live + tombstone entries (used for load check) */
} WgvkHashTable;

static size_t hash_ptr(void* key, size_t bucket_count) {
    size_t k = (size_t)key;
    if (sizeof(size_t) >= 8) {
        k ^= k >> 33;
        k *= (size_t)0xff51afd7ed558ccdULL;
        k ^= k >> 33;
        k *= (size_t)0xc4ceb9fe1a85ec53ULL;
        k ^= k >> 33;
    } else {
        k ^= k >> 16;
        k *= 0x45d9f3bU;
        k ^= k >> 16;
    }
    return k % bucket_count;
}

WgvkHashTable* wgvk_hash_table_create(size_t bucket_count) {
    /* Ensure bucket_count is at least 8 and a power of two for simplicity */
    if (bucket_count < 8) bucket_count = 8;

    WgvkHashTable* table = malloc(sizeof(WgvkHashTable));
    if (!table) return NULL;

    table->buckets = calloc(bucket_count, sizeof(WgvkHashBucket));
    if (!table->buckets) {
        free(table);
        return NULL;
    }

    table->bucket_count = bucket_count;
    table->size         = 0;
    table->occupied     = 0;

    return table;
}

void wgvk_hash_table_destroy(WgvkHashTable* table) {
    if (table) {
        free(table->buckets);
        free(table);
    }
}

/* Grow the table to 2× its current capacity, rehashing all live entries. */
static int hash_table_grow(WgvkHashTable* table) {
    size_t new_count = table->bucket_count * 2;
    WgvkHashBucket* new_buckets = calloc(new_count, sizeof(WgvkHashBucket));
    if (!new_buckets) return -1;

    for (size_t i = 0; i < table->bucket_count; i++) {
        void* k = table->buckets[i].key;
        if (k == NULL || k == WGVK_HT_TOMBSTONE) continue;

        size_t idx = hash_ptr(k, new_count);
        while (new_buckets[idx].key != NULL) {
            idx = (idx + 1) % new_count;
        }
        new_buckets[idx].key   = k;
        new_buckets[idx].value = table->buckets[i].value;
    }

    free(table->buckets);
    table->buckets      = new_buckets;
    table->bucket_count = new_count;
    table->occupied     = table->size; /* tombstones gone after rehash */
    return 0;
}

void wgvk_hash_table_insert(WgvkHashTable* table, void* key, void* value) {
    if (!table || !key || key == WGVK_HT_TOMBSTONE) return;

    /* Grow before inserting if load is too high. */
    if (table->occupied * WGVK_HT_MAX_LOAD_DEN >=
        table->bucket_count * WGVK_HT_MAX_LOAD_NUM) {
        if (hash_table_grow(table) != 0) return; /* OOM — drop insert */
    }

    size_t idx = hash_ptr(key, table->bucket_count);
    size_t first_tombstone = (size_t)-1;

    for (size_t i = 0; i < table->bucket_count; i++) {
        void* k = table->buckets[idx].key;

        if (k == NULL) {
            /* Empty slot — use tombstone slot if found earlier, else this one */
            size_t dest = (first_tombstone != (size_t)-1) ? first_tombstone : idx;
            if (first_tombstone == (size_t)-1) table->occupied++;
            table->buckets[dest].key   = key;
            table->buckets[dest].value = value;
            table->size++;
            return;
        }
        if (k == WGVK_HT_TOMBSTONE) {
            if (first_tombstone == (size_t)-1) first_tombstone = idx;
        } else if (k == key) {
            table->buckets[idx].value = value;
            return;
        }

        idx = (idx + 1) % table->bucket_count;
    }

    /* Table is full of tombstones but we found one earlier — reuse it. */
    if (first_tombstone != (size_t)-1) {
        table->buckets[first_tombstone].key   = key;
        table->buckets[first_tombstone].value = value;
        table->size++;
    }
}

void* wgvk_hash_table_lookup(WgvkHashTable* table, void* key) {
    if (!table || !key || key == WGVK_HT_TOMBSTONE) return NULL;

    size_t idx = hash_ptr(key, table->bucket_count);

    for (size_t i = 0; i < table->bucket_count; i++) {
        void* k = table->buckets[idx].key;
        if (k == NULL) return NULL;          /* empty → key not present */
        if (k == key)  return table->buckets[idx].value;
        idx = (idx + 1) % table->bucket_count;
    }
    return NULL;
}

void wgvk_hash_table_remove(WgvkHashTable* table, void* key) {
    if (!table || !key || key == WGVK_HT_TOMBSTONE) return;

    size_t idx = hash_ptr(key, table->bucket_count);

    for (size_t i = 0; i < table->bucket_count; i++) {
        void* k = table->buckets[idx].key;
        if (k == NULL) return;  /* not found */
        if (k == key) {
            table->buckets[idx].key   = WGVK_HT_TOMBSTONE;
            table->buckets[idx].value = NULL;
            table->size--;
            return;
        }
        idx = (idx + 1) % table->bucket_count;
    }
}
