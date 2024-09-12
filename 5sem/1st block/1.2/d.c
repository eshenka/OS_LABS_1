#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

void* mythread(void* args) {
    // int err = pthread_detach(pthread_self());
    //
    // if (err != 0) {
    //     perror("Failed detach");
    //     pthread_exit(NULL);
    // }

    printf("Thread ID: %lu\n Thread TID: %d\n", pthread_self(), gettid());

    /* pthread_exit(NULL); */

    pthread_detach(pthread_self());
}

int main() {
    pthread_t tid;
    int err;

    while (true) {
        err = pthread_create(&tid, NULL, mythread, NULL);
        if (err != 0) {
            perror("Failed create");
            return -1;
        }

        // err = pthread_join(tid, NULL);
        // if (err != 0) {
        //     perror("Failed join");
        //     return -2;
        // }
    }

}
