#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>

void heap_allocation(int N, int iters) {
    char* buffers[iters];
    for (int i = 0; i < iters; i++) {
        buffers[i] = (char*) malloc(N * sizeof(char));
        //sleep(1);
    }

    sleep(5);

    for (int i = 0; i < iters; i++) {
        free(buffers[i]);
        sleep(1);
    }
}

void add_pages(int pages_number) {
    int page_size = getpagesize();
    
    int* map = mmap(NULL, pages_number * page_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (map == MAP_FAILED) {
        perror("Unable to add pages");
        exit(-1);
    }

    printf("Added pages\n");
    sleep(5);

    int protect = mprotect(map, page_size, PROT_WRITE);
    if (protect == -1) {
        perror("Unable to change mode");
        exit(-1);
    }

    printf("Changed mode\n");
    sleep(5);

    //OK
    *map = 1;

    //SEGFAULT
    //printf("%d\n", map[0]);

    protect = mprotect(map, page_size, PROT_READ);
    if (protect == -1) {
        perror("Unable to change mode");
        exit(-1);
    }

    printf("Changed mode\n");
    sleep(5);

    //SEGFAULT
    //*map = 1;

    //OK
    printf("%d\n", map[0]);

    int unmap = munmap(map + page_size * 3, page_size * 3);
    if (unmap == -1) {
        perror("Unable to unmap pages");
        exit(-1);
    }

    printf("Unmapped pages\n");

    sleep(5);
} 

int main() {
    pid_t pid = getpid();
    printf("PID: %d\n", pid);

    sleep(10);

    int iterations = 1000;
    int size = 204800;
    heap_allocation(size, iterations);

    sleep(5);

    add_pages(10);

    return 0;
}
