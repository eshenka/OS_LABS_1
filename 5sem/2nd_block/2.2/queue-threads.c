#define _GNU_SOURCE
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <bits/pthreadtypes.h>

#include <pthread.h>
#include <sched.h>

#include "queue.h"

#define RED "\033[41m"
#define NOCOLOR "\033[0m"

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

void set_cpu(int n) {
    int err;
    cpu_set_t cpuset;
    pthread_t tid = pthread_self();

    CPU_ZERO(&cpuset);
    CPU_SET(n, &cpuset);

    err = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset);
    if (err) {
        printf("set_cpu: pthread_setaffinity failed for cpu %d\n", n);
        return;
    }

    printf("set_cpu: set cpu %d\n", n);
}

void *reader(void *arg) {
    int expected = 0;
    queue_t *q = (queue_t *)arg;
    printf("reader [%d %d %d]\n", getpid(), getppid(), gettid());

    set_cpu(1);

    while (1) {
        int val = -1;

        pthread_mutex_lock(&mut);

        int ok = queue_get(q, &val);

        pthread_mutex_unlock(&mut);

        if (!ok)
            continue;

        if (expected != val)
            printf(RED "ERROR: get value is %d but expected - %d" NOCOLOR "\n",
                   val, expected);

        expected = val + 1;
    }

    return NULL;
}

void *writer(void *arg) {
    int i = 0;
    queue_t *q = (queue_t *)arg;
    printf("writer [%d %d %d]\n", getpid(), getppid(), gettid());

    set_cpu(1);

    while (1) {
        pthread_mutex_lock(&mut);

        int ok = queue_add(q, i);

        pthread_mutex_unlock(&mut);

        if (!ok)
            continue;
        i++;
    }

    return NULL;
}

int main() {
    pthread_t tid[2];
    queue_t *q;
    int err;

    printf("main [%d %d %d]\n", getpid(), getppid(), gettid());

    q = queue_init(1000000);

    err = pthread_create(&tid[0], NULL, reader, q);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    sched_yield();

    err = pthread_create(&tid[1], NULL, writer, q);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    // TODO: join threads
    for (int i = 0; i < 2; i++) {
        err = pthread_join(tid[i], NULL);
        if (err) {
            printf("main: pthread_join() failed: %s\n", strerror(err));
            return -1;
        }
    }

    pthread_exit(NULL);

    return 0;
}