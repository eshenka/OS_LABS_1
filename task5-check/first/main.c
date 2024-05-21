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

        printf("\nVariables in child process\n");
        printf("global varibale: %p %d\n", &global_var, global_var);
        printf("local variable:  %p %d\n", &local_var, local_var);
        
        global_var = 7;
        local_var = 8;

        printf("\nChanged variables in child process\n");
        printf("global varibale: %p %d\n", &global_var, global_var);
        printf("local variable:  %p %d\n", &local_var, local_var);
        
    }

    if (child_pid != 0) {
    	printf("\nChanged variables in parent process\n");
        printf("global varibale: %p %d\n", &global_var, global_var);
        printf("local variable:  %p %d\n", &local_var, local_var);
        
        sleep(10);
        //exit(0);
    }
    
    if (child_pid == 0) {
        sleep(10);

        pid_t newpid = getppid();

        printf("NEW PID: %d\n", newpid);
        
        raise(SIGSEGV);

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
