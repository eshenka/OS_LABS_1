#ifndef CACHE_H
#define CACHE_H

#include "hashmap.c/hashmap.h"
#include "list.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct CacheEntry {
    char* url;
    List* data;
    int arc;
    size_t response_len;
    bool done;
    int parts_done;

    struct LRUQueue* queue_node;

    pthread_rwlock_t lock;
    pthread_cond_t new_part;
    pthread_mutex_t wait_lock;

} CacheEntry;

typedef struct HashValue {
    char* url;
    CacheEntry* entry;
} HashValue;

typedef struct LRUQueue {
    struct LRUQueue* prev;
    struct LRUQueue* next;
    CacheEntry* entry;
} LRUQueue;

uint64_t my_hash(const void* item, uint64_t seed0, uint64_t seed1);

int my_compare(const void* a, const void* b, void* udata);

struct hashmap* cache_create();

CacheEntry* cache_entry_create(char* url, size_t data_size);
void cache_entry_free(CacheEntry* entry);

HashValue* cache_value_create(CacheEntry* entry);

void cache_entry_add(LRUQueue** queue, CacheEntry* entry);
void cache_entry_upd(LRUQueue** queue, CacheEntry* entry);
CacheEntry* cache_entry_remove(LRUQueue** queue);

#endif // !CACHE_H
