#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int global_initialized = 123456;
int global_uninitialized;
const int global_const = 987654;

void f(int static_var) {
    int local_var = 369852;
    const int local_const_var = 147852;

    printf("static_var:           %p\n", &static_var);
    printf("local var:            %p\n", &local_var);
    printf("local_const_var:      %p\n", &local_const_var);
}

int main(int argc, char *argv[]) {
    f(123);
    printf("global_initialized:   %p\n", &global_initialized);
    printf("global_uninitialized: %p\n", &global_uninitialized);
    printf("global_const:         %p\n", &global_const);

    int procfd = open("/proc/self/maps", O_RDONLY);

    int buffer_size = 256;
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

    return 0;
}
