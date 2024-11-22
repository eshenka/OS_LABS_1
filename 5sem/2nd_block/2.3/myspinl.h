#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>

typedef struct myspin {
    atomic_int lock;
} myspin_t;

int myspin_init(myspin_t* spin);
int myspin_lock(myspin_t* spin);
int myspin_unlock(myspin_t* spin);
