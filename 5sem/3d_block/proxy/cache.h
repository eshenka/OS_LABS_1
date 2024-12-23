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

typedef struct LRUQueue {
    struct LRUQueue* prev;
    struct LRUQueue* next;
    CacheEntry* entry;
} LRUQueue;

uint64_t my_hash(const void* item, uint64_t seed0, uint64_t seed1);

int my_compare(const void* a, const void* b, void* udata);

struct hashmap* create_cache();

CacheEntry* create_entry(char* url, size_t data_size);
void free_entry(CacheEntry* entry);

void add_entry(LRUQueue** queue, CacheEntry* entry);
void upd_entry(LRUQueue** queue, CacheEntry* entry);
CacheEntry* del_entry(LRUQueue** queue);

#endif // !CACHE_H
