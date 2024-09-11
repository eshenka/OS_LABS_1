#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

void* retrun_str(void* arg) {
    char* str = "hello world!\n";

    pthread_exit((void*) str);
}

int main() {
    pthread_t tid;
    int err;
    void* retval;

    err = pthread_create(&tid, NULL, retrun_str, NULL);
    if (err != 0) {
        perror("Failed create");
        return -1;
    }

    err = pthread_join(tid, &retval);
    if (err != 0) {
        perror("Failed join");
        return -2;
    }

    char* ret = (char*) retval;

    printf("Thread had returned: %s", ret);
}
