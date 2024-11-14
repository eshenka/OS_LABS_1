#include <malloc.h>

#include "linked_list.h"
#include "myspinl.h"

struct sync {
    myspin_t spin;
};

void lock_init(Node* node) {
    node->lock = (sync_t*)malloc(sizeof(sync_t));
    myspin_init(&node->lock->spin);
}

void read_lock(Node* node) { myspin_lock(&node->lock->spin); }

void write_lock(Node* node) { myspin_lock(&node->lock->spin); }

void unlock(Node* node) { myspin_unlock(&node->lock->spin); }
