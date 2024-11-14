#define _GNU_SOURCE
#define SPIN 100

#include "mymutexl.h"

#include <linux/futex.h>
#include <pthread.h>
#include <stdatomic.h>
#include <sys/syscall.h>
#include <unistd.h>

static int futex(uint32_t* uaddr, int futex_op, uint32_t val,
                 const struct timespec* timeout, uint32_t* uaddr2,
                 uint32_t val3) {
    return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}

int mymutex_init(mymutex_t* mut) {
    mut->owner = gettid();
    mut->mut = 0;

    return 1;
}

int mymutex_lock(mymutex_t* mut) {
    while (1) {
        for (int i = 0; i < SPIN; i++) {
            int zero = 0;
            if (atomic_compare_exchange_strong(&mut->mut, &zero, 1)) {
                return 1;
            }
        }

        futex((uint32_t*)&mut->mut, FUTEX_WAIT, 1, NULL, NULL, 0);
    }

    return 1;
}

int mymutex_unlock(mymutex_t* mut) {
    int one = 1;
    atomic_compare_exchange_strong(&mut->mut, &one, 0);
    futex((uint32_t*)&mut->mut, FUTEX_WAKE, 1, NULL, NULL, 0);

    return 1;
}
