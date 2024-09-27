#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <stdbool.h>
#include <signal.h>

void sigalrm_handler(int s) {
    write(STDOUT_FILENO, "Caught SIGALRM\n", strlen("Caught SIGALRM\n"));
}

void* thread_SIGALRM(void* args) {
    int err;

    struct sigaction act;
    act.sa_handler = sigalrm_handler;

    err = sigaction(SIGUSR1, &act, NULL);
    if (err != 0) {
        printf("Failed 'sigaction()': %s\n", strerror(err));
        pthread_exit(NULL);
    }

    while (true) {
        sleep(1);
    }

    pthread_exit(NULL);
}

void sigabrt_handler(int s) {
    write(STDOUT_FILENO, "Caught SIGABRT\n", strlen("Caught SIGABRT\n"));
}

void* thread_SIGABRT(void* args) {
    int err;

    struct sigaction act;
    act.sa_handler = sigabrt_handler;

    err = sigaction(SIGUSR2, &act, NULL);
    if (err != 0) {
        printf("Failed 'sigaction()': %s\n", strerror(err));
        pthread_exit(NULL);
    }

    while (true) {
        sleep(1);
    }

    pthread_exit(NULL);
}

int main() {
    int err;
    pthread_t tid_sigalrm;
    pthread_t tid_sigabrt;

    err = pthread_create(&tid_sigalrm, NULL, thread_SIGALRM, NULL);
    if (err != 0) {
        printf("Failed to create sigalrm thread: %s\n", strerror(err));
        return 0;
    }

    err = pthread_create(&tid_sigabrt, NULL, thread_SIGABRT, NULL);
    if (err != 0) {
        printf("Failed to create sigabrt thread: %s\n", strerror(err));
        return 0;
    }

    printf("Sending SIGALRM to sigalrm thread\n");
    err = pthread_kill(tid_sigalrm, SIGUSR1);
    if (err != 0) {
        printf("Failed sending SIGALRM: %s\n", strerror(err));
        return 0;
    }
    sleep(1);

    printf("Sending SIGABRT to sigabrt thread\n");
    err = pthread_kill(tid_sigabrt, SIGUSR2);
    if (err != 0) {
        printf("Failed sending SIGABRT: %s\n", strerror(err));
        return 0;
    }
    sleep(1);

    printf("Sending SIGABRT to sigalrm thread\n");
    err = pthread_kill(tid_sigalrm, SIGUSR2);
    if (err != 0) {
        printf("Failed sending SIGABRT\n");
        return 0;
    }
    sleep(1);

    printf("Sending SIGALRM to sigabrt thread\n");
    err = pthread_kill(tid_sigabrt, SIGUSR1);
    if (err != 0) {
        printf("Failed sending SIGALRM\n");
        return 0;
    }
    sleep(1);

    void* retval;

    pthread_cancel(tid_sigalrm);
    pthread_cancel(tid_sigabrt);

    err = pthread_join(tid_sigalrm, &retval);
    if (err != 0) {
        printf("Failed joining sigalrm: %s\n", strerror(err));
        return 0;
    }

    err = pthread_join(tid_sigabrt, &retval);
    if (err != 0) {
        printf("Failed joining sigabrt: %s\n", strerror(err));
        return 0;
    }

    return 0;
}
