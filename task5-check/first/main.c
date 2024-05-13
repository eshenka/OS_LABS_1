#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int global_var = 123456;

int main() {
    int local_var = 987654;

    printf("global varibale: %p %d\n", &global_var, global_var);
    printf("local var %p %d\n", &local_var, local_var);

    pid_t pid = getpid();
    printf("\nPID: %d\n", pid);

    return 0;
}
