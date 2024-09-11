#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

void* print_strings_nonstop(void* args) {
    while (true) {
        printf("right roung right round\n");
    }
}

int main() {
    pthread_t tid;
    int err;

    err = pthread_create(&tid, NULL, print_strings_nonstop, NULL);
    if (err != 0) {
        perror("Failed create");
        return -1;
    }

    sleep(2);

    err = pthread_cancel(tid);
    if (err != 0) {
        perror("Failed cancel");
        return -3;
    }

    return 0;
}
