#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <limits.h>

int main() {
    int page_size = getpagesize();

    unsigned int* page = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    pid_t child = fork();

    if (child != 0) {
        int i = 0;
        
        for (unsigned int number = 0; number <= UINT_MAX; number++) {
            if (i == page_size / sizeof(unsigned int)) {
                i = 0;
            }

            page[i] = number;
            
            i++;
        }
    }

    if (child == 0) {
        int i = 0;
        int prev_number = page[i];
        i++;

        while(1) {
            if (prev_number == UINT_MAX) {
                break;
            }

            if (i == page_size / sizeof(unsigned int)) {
                i = 0;
            }

            int cur_number = page[i];

            if (cur_number - prev_number != 1) {
                printf("Not contiguous!\n");
            }

            prev_number = cur_number;

            i++;
        }
    }
} 
