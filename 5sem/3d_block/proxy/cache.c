#include "cache.h"
#include "list.h"
#include <malloc.h>
#include <pthread.h>
#include <stdint.h>

CacheEntry* create_entry(char* url, size_t buf_size) {
    CacheEntry* entry = (CacheEntry*)malloc(sizeof(CacheEntry));
    entry->data = create_list(buf_size);
    entry->url = url;
    pthread_rwlock_init(&entry->lock, NULL);
    pthread_cond_init(&entry->new_part, NULL);
    pthread_mutex_init(&entry->wait_lock, NULL);
    entry->arc = 0;
    entry->response_len = 0;
    entry->done = false;
    entry->parts_done = 0;

    return entry;
}

void free_entry(CacheEntry* entry) {
    printf("freeing list\n");
    free_list(entry->data);
    printf("freeing url\n");
    free(entry->url);
    pthread_rwlock_destroy(&entry->lock);
    pthread_cond_destroy(&entry->new_part);
    pthread_mutex_destroy(&entry->wait_lock);

    /*printf("freeing entry %p\n", entry);*/
    /*free(entry);*/
}

uint64_t my_hash(const void* item, uint64_t seed0, uint64_t seed1) {
    const struct CacheEntry* entry = (CacheEntry*)item;
    return hashmap_sip(entry->url, strlen(entry->url), seed0, seed1);
}

int my_compare(const void* a, const void* b, void* udata) {
    const struct CacheEntry* entry_a = (CacheEntry*)a;
    const struct CacheEntry* entry_b = (CacheEntry*)b;

    return strcmp(entry_a->url, entry_b->url);
}

struct hashmap* create_cache() {
    struct hashmap* map = hashmap_new(sizeof(CacheEntry), 0, 0, 0, my_hash,
                                      my_compare, NULL, NULL);

    return map;
}

void add_entry(LRUQueue** queue, CacheEntry* entry) {
    if (*queue == NULL) {
        (*queue) = (LRUQueue*)malloc(sizeof(LRUQueue));
        (*queue)->prev = NULL;
        (*queue)->next = NULL;
        (*queue)->entry = entry;
        entry->queue_node = *queue;

        return;
    }

    LRUQueue* queue_new = (LRUQueue*)malloc(sizeof(LRUQueue));
    (*queue)->prev = queue_new;
    queue_new->next = (*queue);
    queue_new->prev = NULL;

    queue_new->entry = entry;
    entry->queue_node = queue_new;

    (*queue) = queue_new;
}

void upd_entry(LRUQueue** queue, CacheEntry* entry) {
    if (entry->queue_node->prev != NULL) {
        entry->queue_node->prev->next = entry->queue_node->next;
    }

    if (entry->queue_node->next != NULL) {
        entry->queue_node->next->prev = entry->queue_node->prev;
    }
    /*entry->queue_node->prev->next = entry->queue_node->next;*/
    /*entry->queue_node->next->prev = entry->queue_node->prev;*/

    (*queue)->prev = entry->queue_node;
    entry->queue_node->next = *queue;
    entry->queue_node->prev = NULL;

    *queue = entry->queue_node;
}

CacheEntry* del_entry(LRUQueue** queue) {
    LRUQueue** node = queue;
    while ((*node)->next != NULL) {
        (*node) = (*node)->next;
    }

    CacheEntry* entry = (*node)->entry;

    (*node)->prev->next = NULL;
    free(*node);

    return entry;
}
