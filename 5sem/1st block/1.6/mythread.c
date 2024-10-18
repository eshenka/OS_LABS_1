#define _GNU_SOURCE
#include <stdbool.h>
#include "mythread.h"
#include <sys/mman.h>
#include <stdio.h>
#include <sched.h>
#include <ucontext.h>
#include <sys/mman.h>
#include <unistd.h>
#include <setjmp.h>
#include <threads.h>
#include <sys/ucontext.h>
#include <stdlib.h>
#include <signal.h>

#define STACK_SIZE 4096 * 2 * 1024
#define PAGE 4096

thread_local mythread_t thread_global;
thread_local ucontext_t new_context;

void free_detached_thread() {
    munmap((void*) thread_global - STACK_SIZE + sizeof(mythread_struct), STACK_SIZE);
}

int start_routine_wrapper(void* args) {
    mythread_t thread = (mythread_t) args;
    thread_global = thread;

    getcontext(&thread->before_start_routine);

    if (!(thread_global->cancelled) && !setjmp(thread_global->env)) {
        thread_global->retval = thread_global->start_routine(thread_global->args);
    }

    if (thread->detached) {
        getcontext(&new_context);

        char* new_stack = (char*) malloc(PAGE * sizeof(char));

        new_context.uc_stack.ss_sp = new_stack;
        new_context.uc_stack.ss_size = PAGE;
        new_context.uc_stack.ss_flags = 0;
        new_context.uc_link = NULL;

        makecontext(&new_context, free_detached_thread, 0);
        setcontext(&new_context);
    }

    return 0;
}

void* create_stack() {
    int err;

    void* stack = mmap(
        NULL, 
        STACK_SIZE, 
        PROT_NONE, 
        MAP_PRIVATE|MAP_ANONYMOUS|MAP_STACK, 
        -1, 
        0
    );
    if (stack == MAP_FAILED) {
        perror("Failed to create stack");
        return 0;
    }

    err = mprotect(
        stack + PAGE, 
        STACK_SIZE - PAGE, 
        PROT_WRITE|PROT_READ
    );
    if (err != 0) {
        munmap(stack, STACK_SIZE);
        perror("Failed stack protecting");
        return 0;
    }

    return stack;
}

int mythread_create(mythread_t* thread,
                    void *(*start_routine)(void *),
                    void *args) {
    void* thread_stack = create_stack();

    *thread = thread_stack + STACK_SIZE - sizeof(mythread_struct);

    (*thread)->start_routine = start_routine;
    (*thread)->args = args;
    (*thread)->retval = NULL;
    (*thread)->detached = 0;
    (*thread)->cancelled = 0;
    (*thread)->finished = 1;
    (*thread)->joined = 0;

    int err;
    err = clone(
        start_routine_wrapper,
        *thread,
        CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND|CLONE_THREAD|CLONE_SYSVSEM|CLONE_PARENT_SETTID|CLONE_CHILD_CLEARTID,
        (void*) *thread,
        &(*thread)->finished,
        NULL,
        &(*thread)->finished
    );
    if (err == -1) {
        perror("Failed thread create");
        return -3;
    }

    return 0;
}

int mythread_join(mythread_t thread, void **retval) {
    if (thread->detached) {
        printf("Cannot join detached thread");
        return -2;
    }

    while(thread->finished != 0) {
        usleep(1);
    }

    if (retval != NULL) {
        *retval = thread->retval;
    }

    int err = munmap((void*) thread - STACK_SIZE + sizeof(mythread_struct), STACK_SIZE);
    if (err != 0) {
        perror("Failed free stack");
        return -1;
    }

    return 0;
}

int mythread_detach(mythread_t thread) {
    if (thread->detached) {
        printf("Not a joinable thread\n");
        return -1;
    }

    thread->detached = 1;
    return 0;
}

int mythread_cancel(mythread_t thread) {
    thread->cancelled = 1;
    return 0;
}

void mythread_testcancel() {
    if (!thread_global->cancelled) {
        return;
    }
    
    setcontext(&(thread_global->before_start_routine));
}

mythread_t mythread_self() {
    return thread_global;
}

[[noreturn]] void mythread_exit(void* retval) {
    thread_global->retval = retval;
    longjmp(thread_global->env, 1);
}
