#include <pthread.h>

#include "linked_list.h"

struct sync {
    pthread_rwlock_t rwlock;
};

void lock_init(Node* node) { pthread_rwlock_init(&node->lock->rwlock, NULL); }

void read_lock(Node* node) { pthread_rwlock_rdlock(&node->lock->rwlock); }

void write_lock(Node* node) { pthread_rwlock_wrlock(&node->lock->rwlock); }

void unlock(Node* node) { pthread_rwlock_unlock(&node->lock->rwlock); }
