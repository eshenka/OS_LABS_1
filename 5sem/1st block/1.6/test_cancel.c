#include "mythread.h"
#include <stdio.h>
#include <unistd.h>

void* test_cancel() {
    printf("Hello from cancelling thread\n");

    while (true) {
        printf("Waiting for cancelletion\n");
        mythread_testcancel();
        sleep(1);
    }

    mythread_exit(NULL);
}

int main() {
    mythread_t tid_cancel;

    mythread_create(&tid_cancel, test_cancel, NULL);

    sleep(5);

    printf("Cancelling\n");

    mythread_cancel(tid_cancel);


    printf("Joining canclled thread\n");
    mythread_join(tid_cancel, NULL);

    return 0;
}
