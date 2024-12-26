#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "linked_list.h"

#define SEED 6475837

bool cancel = false;
int cnt = 0;

int compared = 0;
int swapped = 0;
int successful = 0;

void swap(Node* parent, Node** first, Node** second) {
    parent->next = *second;
    (*first)->next = (*second)->next;
    (*second)->next = *first;

    Node* tmp = *first;
    *first = *second;
    *second = tmp;
}

int ascend(char* str1, char* str2) { return strlen(str1) < strlen(str2); }

int descend(char* str1, char* str2) { return strlen(str1) > strlen(str2); }

int equal(char* str1, char* str2) { return strlen(str1) == strlen(str2); }

void compare(List* list, int (*order)(char*, char*)) {
    Node* first = list->first;

    read_lock(first);
    while (first->next != NULL) {
        read_lock(first->next);
        Node* second = first->next;

        order(first->value, second->value);

        Node* new_second = second->next;
        unlock(first);

        first = second;
        second = new_second;
    }

    unlock(first);

    __sync_fetch_and_add(&compared, 1);
}

void* ascending_thread(void* list) {
    while (!cancel) {
        compare(list, ascend);
    }

    return NULL;
}

void* descending_thead(void* list) {
    while (!cancel) {
        compare(list, descend);
    }

    return NULL;
}

void* equality_thread(void* list) {
    while (!cancel) {
        compare(list, equal);
    }

    return NULL;
}

void* swap_thread(void* list_v) {
    unsigned int seed = 98453;

    while (!cancel) {
        List* list = (List*)list_v;

        Node* first = list->first;
        write_lock(first);

        Node* second = first->next;
        write_lock(second);

        Node* third = second->next;

        while (third != NULL) {
            write_lock(third);

            int rand_v = rand_r(&seed);

            if (rand_v < (RAND_MAX / 2)) {
                if (cnt <= 10) {
                    printf("v = %d\n", rand_v);
                    cnt++;
                }
                __sync_fetch_and_add(&successful, 1);
                swap(first, &second, &third);
            }

            Node* new_third = third->next;

            unlock(first);

            first = second;
            second = third;
            third = new_third;
        }

        unlock(first);
        unlock(second);

        __sync_fetch_and_add(&swapped, 1);
    }

    return NULL;
}

int main(int argc, char* argv[]) {
    srand(SEED);

    List* list = create_list(atoi(argv[1]));

    printf("%s\n%s\n%s\n", list->first->value, list->first->next->value,
           list->first->next->next->value);

    pthread_t tids[6];

    pthread_create(&tids[0], NULL, ascending_thread, list);
    pthread_create(&tids[1], NULL, descending_thead, list);
    pthread_create(&tids[2], NULL, equality_thread, list);

    for (int i = 3; i < 6; i++) {
        pthread_create(&tids[i], NULL, swap_thread, list);
    }

    sleep(10);

    cancel = true;

    for (int i = 0; i < 6; i++) {
        pthread_join(tids[i], NULL);
    }

    printf("compared = %d\nswapped = %d\nrelation = %f\nsuccessful = %d\n",
           compared, swapped, (float)compared / swapped, successful);

    destroy_list(list);

    return 0;
}
