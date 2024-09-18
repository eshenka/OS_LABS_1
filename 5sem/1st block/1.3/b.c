#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>

typedef struct Mystruct {
    int integer;
    char* str;
} Mystruct;

void* print_struct(void* args) {
    int err = pthread_detach(pthread_self());
    if (err != 0) {
        perror("Failed detach");
        pthread_exit(NULL);
    }

    Mystruct* object = (Mystruct*) args;

    printf("integer = %d\nstring = %s\n", object->integer, object->str);

    pthread_exit(NULL);
}

int main() {
    Mystruct* object = (Mystruct*) malloc(sizeof(Mystruct));
    object->integer = 123685555;
    object->str = "hello world!!!!";

    pthread_t tid;
    int err;

    err = pthread_create(&tid, NULL, print_struct, object);
    if (err != 0) {
        perror("Failed create");
        return -1;
    }

    pthread_exit(NULL);
}
