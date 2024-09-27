#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int GLOBAL_VALUE = 123456;

void *mythread(void *arg) {
    printf("mythread: gettid() = %d\n", gettid());
    printf("mythread: pthread_self() = %lu\n", pthread_self());

    static int static_thread = 125478;
    const int const_thread = 123654;
    int local_thread = 521478;

    printf("thread_static = %p\n", &static_thread);
    printf("thread_const = %p\n", &const_thread);
    printf("thred_local = %p\n", &local_thread);
    printf("thread_global = %p\n", &GLOBAL_VALUE);

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

    static int static_main = 147852;
    const int const_main = 523986;
    int local_main = 123654;

    printf("main_static = %p\n", &static_main);
    printf("main_const = %p\n", &const_main);
    printf("main_local = %p\n", &local_main);
    printf("main_global = %p\n", &GLOBAL_VALUE);

    err = pthread_join(tid, NULL);
    if (err != 0) {
        perror("Failed");
        return -1;
    }

	return 0;
}
