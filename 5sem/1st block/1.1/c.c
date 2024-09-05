#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void *mythread(void *arg) {
    printf("mythread: gettid() = %d\n", gettid());
    printf("mythread: pthread_self() = %lu\n", pthread_self());

	return NULL;
}

int main() {
	pthread_t tid;
	int err;

	printf("main [pid = %d ppid = %d ttid = %d]\n", getpid(), getppid(), gettid());

	err = pthread_create(&tid, NULL, mythread, NULL);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

    printf("main: pthread_create tid = %lu\n", tid);

    pthread_t main_tid = gettid();
    if (main_tid == tid) {
        printf("equal\n");
    } else {
        printf("not equal\n");
    }

    err = pthread_join(tid, NULL);
    if (err != 0) {
        perror("Failed");
        return -1;
    }

	return 0;
}
