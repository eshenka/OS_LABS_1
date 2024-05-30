#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    printf("Allocating memory for buffer...\n");
    char* buffer = (char*) malloc (100 * sizeof(char));

    printf("Writing a phrase...\n");
    memcpy(buffer, "hello world", 11);

    printf("Printig buffer...\n\n");
    write(1, buffer, 100);
    printf("\n\n");

    printf("Free buffer...\n");
    free(buffer);

    printf("Printing freed buffer...\n\n");
    write(1, buffer, 100);
    printf("\n\n");

    printf("Allocating memory for another buffer...\n");
    buffer = (char*) malloc (100 * sizeof(char));

    printf("Writing a phrase...\n");
    memcpy(buffer, "hello world", 11);

    printf("Printing buffer...\n\n");
    write(1, buffer, 100);
    printf("\n\n"); 

    printf("Free buffer middle...\n");
    free(buffer + 50);

    printf("Printing half-freed buffer...\n\n");
    write(1, buffer, 100);
    printf("\n\n");
    
    return 0;
}
