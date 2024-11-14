#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "linked_list.h"

/*#define CAPACITY 100*/

/*int ascending_str_len = 0;*/
/*int descending_str_len = 0;*/
/*int same_str_len = 0;*/
int compared = 0;
int swapped = 0;

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
    while (1) {
        compare(list, ascend);
    }
}

void* descending_thead(void* list) {
    while (1) {
        compare(list, descend);
    }
}

void* equality_thread(void* list) {
    while (1) {
        compare(list, equal);
    }
}

void* swap_thread(void* list_v) {
    while (1) {
        List* list = (List*)list_v;

        if (list->first->next == NULL) {
            printf("Oh oh\n");
        }

        srand(time(NULL));

        Node* first = list->first;
        write_lock(first);

        Node* second = first->next;
        write_lock(second);

        Node* third = second->next;

        while (third != NULL) {
            write_lock(third);

            if (rand() < (RAND_MAX / 2)) {
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
}

int main(int argc, char* argv[]) {
    /*printf("PID = %d\n", getpid());*/
    /**/
    /*sleep(10);*/

    List* list = create_list(atoi(argv[1]));

    pthread_t tids[6];

    pthread_create(&tids[0], NULL, ascending_thread, list);
    pthread_create(&tids[1], NULL, descending_thead, list);
    pthread_create(&tids[2], NULL, equality_thread, list);

    for (int i = 3; i < 6; i++) {
        pthread_create(&tids[i], NULL, swap_thread, list);
    }

    while (1) {
        sleep(1);

        printf("compared = %d\nswapped = %d\nrelation = %f\n\n", compared,
               swapped, (float)compared / swapped);
    }
}
