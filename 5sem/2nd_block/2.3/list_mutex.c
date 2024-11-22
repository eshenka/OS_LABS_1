#include <malloc.h>
#include <pthread.h>
#include <stdio.h>

#include "linked_list.h"

struct sync {
    pthread_mutex_t mut;
};

void lock_init(Node* node) {
    node->lock = (sync_t*)malloc(sizeof(sync_t));
    pthread_mutex_init(&node->lock->mut, NULL);
}

void read_lock(Node* node) { pthread_mutex_lock(&node->lock->mut); }

void write_lock(Node* node) { pthread_mutex_lock(&node->lock->mut); }

void unlock(Node* node) { pthread_mutex_unlock(&node->lock->mut); }
