#include <malloc.h>

#include "linked_list.h"
#include "mymutexl.h"

struct sync {
    mymutex_t mut;
};

void lock_init(Node* node) {
    node->lock = (sync_t*)malloc(sizeof(sync_t));
    mymutex_init(&node->lock->mut);
}

void read_lock(Node* node) { mymutex_lock(&node->lock->mut); }

void write_lock(Node* node) { mymutex_lock(&node->lock->mut); }

void unlock(Node* node) { mymutex_unlock(&node->lock->mut); }
