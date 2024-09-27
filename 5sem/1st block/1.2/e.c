#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

void* mythread(void* args) {
    printf("Thread ID: %lu\n Thread TID: %d\n", pthread_self(), gettid());

    pthread_exit(NULL);
}

int main() {
    int err;

    pthread_attr_t attr;
    err = pthread_attr_init(&attr);
    if (err != 0) {
        perror("Failed attr init");
        return -3;
    }

    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_t tid;

    while (true)  {
        err = pthread_create(&tid, &attr, mythread, NULL);
        if (err != 0) {
            perror("Failed create");
            return -1;
        }
    }

    pthread_exit(NULL);
}
