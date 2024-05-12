#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

void recursive_allocation(int N) {
    char* buffer = (char*) malloc(N * sizeof(char));

    recursive_allocation(N);
}

int main() {
    pid_t pid = getpid();

    printf("PID: %d\n", pid);

    unsigned int seconds = 20;
    sleep(seconds);

    recursive_allocation(4096);

    return 0;
}
