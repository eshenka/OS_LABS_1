#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

#include "linked_list.h"

const char* string =
    "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

Node* create_node() {
    Node* node = (Node*)malloc(sizeof(Node));

    int string_len = 97 - (rand() % 97) + 1;
    for (int i = 0; i < string_len; i++) {
        int key = rand() % string_len;
        node->value[i] = string[key];
    }

    node->next = NULL;
    lock_init(node);

    return node;
}

List* create_list(int len) {
    List* list = (List*)malloc(sizeof(List));
    list->first = NULL;

    for (int i = 0; i < len; i++) {
        Node* new_node = create_node();

        if (list->first != NULL) {
            list->first = new_node;
        } else {
            new_node->next = list->first;
            list->first = new_node;
        }
    }

    return list;
}
