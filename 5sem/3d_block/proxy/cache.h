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

    pthread_rwlock_t lock;

} CacheEntry;

typedef struct LRUQueue {
    int place;
    CacheEntry* entry;
    struct LRUQueue* next;
} LRUQueue;

struct User {
    char* name;
    int age;
};

uint64_t my_hash(const void* item, uint64_t seed0, uint64_t seed1);

int my_compare(const void* a, const void* b, void* udata);

struct hashmap* create_cache();

CacheEntry* create_entry(char* url, size_t data_size);
