// #ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct sync sync_t;

typedef struct _Node {
    char value[100];
    struct _Node* next;
    sync_t* lock;
} Node;

typedef struct _List {
    Node* first;
} List;

List* create_list(int len);
Node* create_node();

void lock_init(Node* node);
void read_lock(Node* node);
void write_lock(Node* node);
void unlock(Node* node);

void destroy_list(List* list);
