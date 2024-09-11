#include <stdio.h>
#include <stdlib.h>

int* get_pointer_to_local_var() {
    int local_var = 123456;
    return &local_var;
}

int main() {
    int* local_var_pointer = get_pointer_to_local_var();

    printf("pointer: %p\n, value: %d\n", local_var_pointer, *local_var_pointer);

    return 0;
}
