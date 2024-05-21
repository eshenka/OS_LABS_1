#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int global_var = 123456;

int main() {
    int local_var = 987654;

    printf("Variables in parent process\n");
    printf("global varibale: %p %d\n", &global_var, global_var);
    printf("local variable:  %p %d\n", &local_var, local_var);

    pid_t pid = getpid();
    printf("\nPID: %d\n", pid);

    pid_t child_pid = fork();
    if (child_pid != 0) {
        printf("child PID: %d\n", child_pid);
    }

    if (child_pid == 0) {
        pid_t parent_pid = getppid(); 

        printf("\nchild PID: %d\n", child_pid);
        printf("parent PID: %d\n", parent_pid);

        pid_t grandcild = fork();

        if (grandcild == 0) {

            printf("%d\n", getppid());
            sleep(5);

            pid_t newparent = getppid();
            printf("NEW PARENT PID: %d\n", newparent);

            sleep(50);
        }
    }


    if (child_pid != 0) {
        sleep(40);

        exit(0);
    }
    
    if (child_pid == 0) {
    	exit(5);
    }

    if (child_pid != 0) {
        int status;
        waitpid(child_pid, &status, WNOHANG);
        printf("\nStatus %d\n", status);

        if (WIFEXITED(status)) {
            int signal = WEXITSTATUS(status);
            printf("\nsignal = %d\n", signal);
        }
    }

    

    return 0;
}
