#include <stdbool.h>
#include <ucontext.h>
#include <setjmp.h>

typedef struct mythread {
    int task_id;

    void* (*start_routine)(void*);
    void* args;
    ucontext_t before_start_routine;

    void* retval;

    int detached;
    int cancelled;
    int finished;
    int joined;

    jmp_buf env;
} mythread_struct;

typedef mythread_struct* mythread_t;


int mythread_create(mythread_t* thread,
    void* (*start_routine)(void*),
    void* args);

int mythread_join(mythread_t thread,
    void** retval);

int mythread_cancel(mythread_t thread);

void mythread_testcancel();

int mythread_detach(mythread_t thread);

[[noreturn]] void mythread_exit(void* retval);
