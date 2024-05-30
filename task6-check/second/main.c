#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define _GNU_SOURCE

int main() {
    int pipefd[2];

    pipe(pipefd);

    pid_t child = fork();

    if (child != 0) {
        for (unsigned int number = 0; number <= UINT_MAX; number++) {
            write(pipefd[1], &number, sizeof(number));
        }
    } else {
        int i = 0;
        unsigned int prev_num;
        unsigned int cur_num;

        read(pipefd[0], &prev_num, sizeof(prev_num));

        while(1) {
            if (i == UINT_MAX) {
                break;
            }

            read(pipefd[0], &cur_num, sizeof(cur_num));

            if (cur_num - prev_num != 1) {
                printf("Not contiguous!\n");
            } else {
                printf("Contiguous!");
            } 

            prev_num = cur_num;

            i++;
        }
    }
    
}
