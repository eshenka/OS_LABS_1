#include <malloc.h>
#include <pthread.h>

#include "linked_list.h"

struct sync {
    pthread_spinlock_t spin;
};

void lock_init(Node* node) {
    node->lock = (sync_t*)malloc(sizeof(sync_t));
    pthread_spin_init(&node->lock->spin, 0);
}

void read_lock(Node* node) { pthread_spin_lock(&node->lock->spin); }

void write_lock(Node* node) { pthread_spin_lock(&node->lock->spin); }

void unlock(Node* node) { pthread_spin_unlock(&node->lock->spin); }
