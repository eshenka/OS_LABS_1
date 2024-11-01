#include "mythread.h"
#include <stdio.h>
#include <unistd.h>

void *thread_join(void *arg) {
    int ret = 42;

    return (void *)(long)ret;
}

int main() {
    int err;
    mythread_t tid_join;
    mythread_t tid_cancel;

    err = mythread_create(&tid_join, thread_join, NULL);
    if (err) {
        printf("Failed to create joinable thread\n");
        return 0;
    }

    err = mythread_create(&tid_cancel, thread_join, NULL);

    void *retval;
    void *retval2;
    err = mythread_join(tid_join, &retval);
    err = mythread_join(tid_cancel, &retval2);

    printf("Joined\n");
    long ret = (long)retval;
    long ret2 = (long)retval2;
    printf("%ld %ld\n", ret, ret2);

    return 0;
}
