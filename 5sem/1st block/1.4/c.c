#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <malloc.h>

void deallocator(void* arg) {
    char* str = (char*) arg;

    printf("String to free: %.5s\n", str);

    free(arg);
}

void* print_nonstop(void* args) {
    char* str = (char*) malloc(sizeof(char) * strlen("Hello World! "));
    strcpy(str, "Hello World!");

    pthread_cleanup_push(deallocator, (void*) str);

    while (true) {
        printf("%s\n", str);
    }

    pthread_cleanup_pop(1);
}

int main() {
    pthread_t tid;
    int err;

    err = pthread_create(&tid, NULL, print_nonstop, NULL);
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
