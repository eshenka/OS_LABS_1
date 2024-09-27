#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct Mystruct {
    int integer;
    char* str;
} Mystruct;

void* print_struct(void* args) {
    sleep(1);

    Mystruct* object = (Mystruct*) args;

    printf("integer = %d\nstring = %s\n", object->integer, object->str);

    printf("Exiting joinable thread...\n");
    pthread_exit(NULL);
}

int main() {
    Mystruct object = {
        .integer = 123486,
        .str = "hello world",
    };

    pthread_t tid;
    int err;

    err = pthread_create(&tid, NULL, print_struct, &object);
    if (err != 0) {
        perror("Failed create");
        return -1;
    }

    err = pthread_join(tid, NULL);
    if (err != 0) {
        perror("Failed join");
        return -2;
    }

    printf("Exiting main...\n");
    pthread_exit(NULL);
}
