#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

void* print_strings_nonstop(void* args) {
    while (true) {
        printf("right round right round\n");
    }
}

int main() {
    pthread_t tid;
    int err;

    err = pthread_create(&tid, NULL, print_strings_nonstop, NULL);
    if (err != 0) {
        printf("Failed thread create: %s\n", strerror(err));
        return -1;
    }

    sleep(2);

    err = pthread_cancel(tid);
    if (err != 0) {
        printf("Failed cancelling thread: %s\n", strerror(err));
        return -3;
    }

    void* cancel_state;

    err = pthread_join(tid, &cancel_state);
    if (err != 0) {
        printf("Failed to join a thread: %s\n", strerror(err));
        return -2;
    }

    if (cancel_state == PTHREAD_CANCELED) {
        printf("Thread was cancelled\n");
    } else {
        printf("Thread was not cancelled\n");
    }

    pthread_exit(NULL);
}
