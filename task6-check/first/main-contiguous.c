#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <limits.h>

int main() {
    int page_size = getpagesize();

    unsigned int* page = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);


    page[0] = 1;
    int flag = 1;

  
    if (page == MAP_FAILED) {
        perror("Unable to map");
        exit(-1);
    }

    pid_t child = fork();

    if (child != 0) {
        int i = 1;
        
        for (unsigned int number = 0; number <= UINT_MAX; number++) {
            if (i == page_size / sizeof(unsigned int)) {
                i = 1;
            }

            while (page[0] != 1) {
            }

            page[i] = number;
            if (flag) {
                i++;
                number++;
                page[i] = number;
                flag = 0;
            }
            page[0] = 2;

            
            i++;
        }
    }

    if (child == 0) {
        int i = 1;
        int prev_number = page[i];

        i++;

        while(1) {
            if (prev_number == UINT_MAX) {
                break;
            }

            if (i == page_size / sizeof(unsigned int)) {
                i = 1;
            }

            while (page[0] != 2) {
            }

            int cur_number = page[i];

            if (cur_number - prev_number != 1) {
                printf("Not contiguous!\n");
            } else {
                printf("Contiguous!\n");
            }

            prev_number = cur_number;


            page[0] = 1;

            i++;
        }
    }
} 
