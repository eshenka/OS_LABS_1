#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[], char* argp[]) {
    pid_t pid = getgid();

    unsigned int seconds = 1;
    sleep(seconds);

    const char filename[] = "part1";

    printf("\nPRINTING PROC SELF MAPS\n");
    
    int procfd = open("/proc/self/maps", O_RDONLY);
    const int buffer_size = 256;
    char buffer[buffer_size];
    while (1) {
        int read_size = read(procfd, buffer, buffer_size - 1);
        if (read_size == 0) {
            break;
        }

        if (read_size == -1) {
            perror("Unable to read file");
            exit(-1);
        }

        buffer[read_size] = '\0';
        printf("%s", buffer);
    }


    execve(filename, argv, argp);

    printf("Hello world!\n");
    
    return 0;
}

