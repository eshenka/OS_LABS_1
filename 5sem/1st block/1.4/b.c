#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

void* count_nonstop(void* args) {
    int err = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    if (err != 0) {
        printf("Failed set cancel state: %s\n", strerror(err));
        pthread_exit(NULL);
    }

    int counter = 0;

    while (true) {
        counter += 1;
    }
}

int main() {
    pthread_t tid;
    int err;

    err = pthread_create(&tid, NULL, count_nonstop, NULL);
    if (err != 0) {
        perror("Failed create");
        return -1;
    }

    err = pthread_cancel(tid);
    if (err != 0) {
        printf("Failed cancel: %s\n", strerror(err));
        return -3;
    }

    void* retval;
    err = pthread_join(tid, &retval);

    if (err != 0) {
        printf("Failed join: %s\n", strerror(err));
        return -2;
    }

    if (retval == PTHREAD_CANCELED) {
        printf("Thread was cancelled\n");
    } else {
        printf("Thread was not cancelled\n");
    }

    return 0;
}
