#define _GNU_SOURCE
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void *mythread(void *arg) {
    printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(),
           gettid());
    return NULL;
}

int main() {
    pthread_t tid;
    int err;

    printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(),
           gettid());

    printf("HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH\n");
    err = pthread_create(&tid, NULL, mythread, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }
    printf("HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH\n");

    err = pthread_join(tid, NULL);
    if (err != 0) {
        perror("Failed");
        return -1;
    }

    return 0;
}
