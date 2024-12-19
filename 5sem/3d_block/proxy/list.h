#ifndef LIST_H
#define LIST_H

#include <stdio.h>

typedef struct List {
    char* buffer;
    struct List* next;
} List;

List* create_list(size_t buf_size);

void add_new_node(List* last_node, size_t buf_size);

void free_list(List* node);

#endif // !LIST_H
