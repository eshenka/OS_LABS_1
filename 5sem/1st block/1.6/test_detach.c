#include "mythread.h"
#include <stdio.h>
#include <unistd.h>

void* test_deatch() {
    mythread_detach(mythread_self());

    printf("Hello from detached thread\n");

    sleep(3);

    printf("Exiting detached thread\n");

    mythread_exit(NULL);
}

int main() {
    mythread_t tid_detached;

    mythread_create(&tid_detached, test_deatch, NULL);

    sleep(10);

    return 0;
}
