#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>

typedef struct mymutex {
    pthread_t owner;
    atomic_int mut;
} mymutex_t;

int mymutex_init(mymutex_t* mut);
int mymutex_lock(mymutex_t* mut);
int mymutex_unlock(mymutex_t* mut);
