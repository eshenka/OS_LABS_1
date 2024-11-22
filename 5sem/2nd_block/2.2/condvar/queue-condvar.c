#define _GNU_SOURCE
#include <assert.h>
#include <pthread.h>

#include "queue-condvar.h"

#define WRITE 1
#define READ 2

void *qmonitor(void *arg) {
    queue_t *q = (queue_t *)arg;

    printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

    while (1) {
        queue_print_stats(q);
        sleep(1);
    }

    return NULL;
}

queue_t *queue_init(int max_count) {
    int err;

    queue_t *q = malloc(sizeof(queue_t));
    if (!q) {
        printf("Cannot allocate memory for a queue\n");
        abort();
    }

    pthread_mutex_init(&q->mut, NULL);
    pthread_cond_init(&q->cond_get, NULL);
    pthread_cond_init(&q->cond_add, NULL);

    q->first = NULL;
    q->last = NULL;
    q->max_count = max_count;
    q->count = 0;

    q->add_attempts = q->get_attempts = 0;
    q->add_count = q->get_count = 0;

    err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);
    if (err) {
        printf("queue_init: pthread_create() failed: %s\n", strerror(err));
        abort();
    }

    return q;
}

void queue_destroy(queue_t *q) {
    int err;
    err = pthread_cancel(q->qmonitor_tid);
    if (err) {
        printf("queue_destroy: pthread_cancel() failed: %s\n", strerror(err));
        abort();
    }

    err = pthread_join(q->qmonitor_tid, NULL);
    if (err) {
        printf("queue_destroy: pthread_join() failed: %s\n", strerror(err));
        abort();
    }

    qnode_t *cur = q->first;
    while (cur) {
        qnode_t *next = cur->next;
        free(cur);
        cur = next;
    }
}

int queue_add(queue_t *q, int val) {
    qnode_t *new = malloc(sizeof(qnode_t));
    if (!new) {
        printf("Cannot allocate memory for new node\n");
        abort();
    }

    new->val = val;
    new->next = NULL;

    pthread_mutex_lock(&q->mut);

    while (q->count == q->max_count) {
        pthread_cond_wait(&q->cond_add, &q->mut);
    }

    q->add_attempts++;

    if (!q->first)
        q->first = q->last = new;
    else {
        q->last->next = new;
        q->last = q->last->next;
    }

    q->count++;
    q->add_count++;

    pthread_cond_signal(&q->cond_get);
    pthread_mutex_unlock(&q->mut);

    return 1;
}

int queue_get(queue_t *q, int *val) {
    pthread_mutex_lock(&q->mut);
    while (q->count == 0) {
        pthread_cond_wait(&q->cond_get, &q->mut);
    }

    q->get_attempts++;

    qnode_t *tmp = q->first;

    *val = tmp->val;
    q->first = q->first->next;

    q->count--;
    q->get_count++;

    pthread_cond_signal(&q->cond_add);
    pthread_mutex_unlock(&q->mut);

    free(tmp);

    return 1;
}

void queue_print_stats(queue_t *q) {
    pthread_mutex_lock(&q->mut);
    printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld "
           "%ld %ld)\n",
           q->count, q->add_attempts, q->get_attempts,
           q->add_attempts - q->get_attempts, q->add_count, q->get_count,
           q->add_count - q->get_count);
    pthread_mutex_unlock(&q->mut);
}
