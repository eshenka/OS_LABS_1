#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

#include "linked_list.h"

unsigned int seed = 9011;

const char* string =
    "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

Node* create_node() {
    Node* node = (Node*)malloc(sizeof(Node));

    unsigned int string_len = 97 - (rand_r(&seed) % 97) + 1;

    for (unsigned int i = 0; i < string_len; i++) {
        int key = rand_r(&seed) % string_len;
        node->value[i] = string[key];
    }
    node->value[string_len] = '\0';

    node->next = NULL;
    lock_init(node);

    return node;
}

List* create_list(int len) {
    List* list = (List*)malloc(sizeof(List));
    list->first = NULL;

    for (int i = 0; i < len; i++) {
        Node* new_node = create_node();

        if (list->first == NULL) {
            list->first = new_node;
        } else {
            new_node->next = list->first;
            list->first = new_node;
        }
    }

    return list;
}

void destroy_node(Node* node) {
    free(node->lock);
    free(node);
}

void destroy_list(List* list) {
    Node* first = list->first;

    while (first != NULL) {
        Node* tmp = first->next;
        destroy_node(first);
        first = tmp;
    }

    free(list);
}
