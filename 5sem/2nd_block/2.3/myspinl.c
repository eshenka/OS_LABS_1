#include <stdatomic.h>
#include <stdio.h>

#include "myspinl.h"

int myspin_init(myspin_t* spin) {
    spin->lock = 0;
    return 1;
}

int myspin_lock(myspin_t* spin) {
    while (1) {
        int zero = 0;
        if (atomic_compare_exchange_strong(&spin->lock, &zero, 1)) {
            break;
        }
    }
    return 1;
}

int myspin_unlock(myspin_t* spin) {
    int one = 1;
    atomic_compare_exchange_strong(&spin->lock, &one, 0);

    return 1;
}
