#include "mythread.h"
#include <stdio.h>
#include <unistd.h>

void* thread_fn(void* args) {
    printf("hello from new thread\n");

    mythread_exit((void*) 42); 
}

int main() {
    mythread_t tid;
    int err;

    err = mythread_create(&tid, thread_fn, NULL);

    sleep(2);

    void* retval;

    printf("%p\n", tid);

    err = mythread_join(tid, &retval);

    int* ret = (int*) retval;

    printf("%d\n", *ret);
    return 0;
}
