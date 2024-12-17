#include <malloc.h>
#include <stdio.h>

#include "list.h"

List* create_list(size_t buf_size) {
    List* list = (List*)malloc(sizeof(List));
    list->buffer = (char*)malloc(sizeof(char) * buf_size);
    list->next = NULL;

    return list;
}

void add_new_node(List* last_node, size_t buf_size) {
    List* new_node = (List*)malloc(sizeof(List));
    new_node->buffer = (char*)malloc(sizeof(char) * buf_size);
    new_node->next = NULL;

    last_node->next = new_node;
}
