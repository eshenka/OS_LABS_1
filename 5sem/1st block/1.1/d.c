#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

int GLOBAL_VALUE = 123456;

typedef struct Args {
    int local_value;
} Args;

void *mythread(void *args) {
    pid_t pid = getpid();

    printf("\n\nPID = %d\n\n", pid);

    Args* values = args;
    
    static int static_thread = 1245;
    const int const_thread = 147855;
    int local_thread = 477744;

    printf("thread_static = %p\n", &static_thread);
    printf("thread_const = %p\n", &const_thread);
    printf("thred_local = %p\n", &local_thread);
    printf("thread_global = %p\n", &GLOBAL_VALUE);
    printf("thread_local_value_pointer = %p\n", &values->local_value);

    sleep(3);

    values->local_value = 0;
    GLOBAL_VALUE = 0;

	return NULL;
}

int main() {
	pthread_t tid;
	int err;

    Args args;
    args.local_value = 147852;

	err = pthread_create(&tid, NULL, mythread, &args);
	if (err) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

    printf("\n\nCHILD TID = %lu\n\n", tid);
    printf("\n\nMAIN TID = %d\n\n", gettid());

    static int static_main = 147852;
    const int const_main = 523986;
    int local_main = 123654;

    printf("main_static = %p\n", &static_main);
    printf("main_const = %p\n", &const_main);
    printf("main_local = %p\n", &local_main);
    printf("main_global = %p\n", &GLOBAL_VALUE);



    printf("global_unchaged = %d\n", GLOBAL_VALUE);
    printf("local_unchaged = %d\n", args.local_value);
    printf("local_value_pointer = %p\n", &args.local_value);
    sleep(5);
    printf("global_changed = %d\n", GLOBAL_VALUE);
    printf("local_changed = %d\n", args.local_value);


    err = pthread_join(tid, NULL);
    if (err != 0) {
        perror("Failed");
        return -1;
    }

    sleep(60);

	return 0;
}



