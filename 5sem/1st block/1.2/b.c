#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>


void* return_number(void* arg) {
    int number = 42;
    pthread_exit((void*)(long) number);
}

int main() {
    pthread_t tid;
    int err;
    void* retval;

    err = pthread_create(&tid, NULL, return_number, NULL);

    if (err != 0) {
        perror("Failed create");
        return -1;
    }

    err = pthread_join(tid, &retval);

    if (err != 0) {
        perror("Failed join");
        return -2;
    }

    long ret = (long) retval;

    printf("Thread has returned %ld\n", ret);

    return 0;
}

