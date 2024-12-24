#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <assert.h>

#include "cache.h"
#include "hashmap.c/hashmap.h"
#include "http-parse.h"
#include "list.h"
#include "network.h"

typedef enum Error { SUCCESS, ERROR_REQUEST_UNSUPPORT } Error;

#define BUFFER_SIZE 8192
#define MAX_BUFFER_PARTS 8
#define URL_SIZE 1024
#define MAX_URL_SIZE 1024 * 8
#define MAX_CACHE_SIZE 5
#define PORT 2367
#define METHOD_SIZE 4

struct hashmap* cache;
LRUQueue* queue_head;
pthread_rwlock_t cache_lock = PTHREAD_RWLOCK_INITIALIZER;
size_t cache_size;

typedef struct RemoteServer {
    char* url;
    CacheEntry* entry;
    size_t request_len;
    char* client_request;
} RemoteServer;

RemoteServer* create_remote_data(CacheEntry* entry, size_t request_len,
                                 char* url, char* client_request) {
    RemoteServer* remote_data = (RemoteServer*)malloc(sizeof(RemoteServer));
    remote_data->url = url;
    remote_data->entry = entry;
    remote_data->client_request = client_request;
    remote_data->request_len = request_len;

    return remote_data;
}

Error check_request(char* method, int minor_version) {
    if (strcmp(method, "GET")) {
        printf("Only GET method is supported\n");
        return ERROR_REQUEST_UNSUPPORT;
    }

    if (minor_version != 0) {
        printf("Only 1.0 version is supported\n");
        return ERROR_REQUEST_UNSUPPORT;
    }

    return SUCCESS;
}

void* handle_remote_server(void* arg) {
    RemoteServer* data = (RemoteServer*)arg;
    char* url = data->url;
    CacheEntry* entry = data->entry;
    size_t request_len = data->request_len;
    char* client_request = data->client_request;

    // Server thread has a ref to entry
    __sync_fetch_and_add(&entry->arc, 1);

    char host[URL_SIZE], path[URL_SIZE];
    sscanf(url, "http://%[^/]%s", host, path);

    struct hostent* server;
    server = gethostbyname(host);

    int server_sockfd = connect_to_remote_server(server);
    if (server_sockfd == -1) {
        int arc = __sync_fetch_and_sub(&entry->arc, 1);
        printf("[ERROR] Unable to connect to a remote server\n");
        free(data);

        if (arc == 1) {
            cache_entry_free(entry);
        }

        return NULL;
    }

    size_t written = 0;
    while (written < request_len) {
        written += write(server_sockfd, client_request + written,
                         request_len - written);
    }

    size_t response_len = 0;

    HTTP_PARSE parse_err = http_parse_read_response(server_sockfd, BUFFER_SIZE,
                                                    &response_len, entry);
    if (parse_err == PARSE_ERROR) {
        int arc = __sync_fetch_and_sub(&entry->arc, 1);
        free(data);

        if (arc == 1) {
            cache_entry_free(entry);
        }

        close(server_sockfd);
        return NULL;
    }

    pthread_rwlock_wrlock(&entry->lock);
    entry->response_len = response_len;
    int arc = __sync_fetch_and_sub(&entry->arc, 1);
    pthread_rwlock_unlock(&entry->lock);

    if (arc == 1) {
        cache_entry_free(entry);
    }
    free(data);
    return NULL;
}

void* handle_client(void* arg) {
    Error err;

    int* client_sockfd = (int*)arg;

    size_t request_len = 0;
    char client_request[BUFFER_SIZE];
    char method[METHOD_SIZE];
    char* url = (char*)malloc(sizeof(char) * URL_SIZE);
    int minor_version;

    HTTP_PARSE parse_err;
    parse_err = http_parse_read_request(
        *client_sockfd, client_request, BUFFER_SIZE, method, METHOD_SIZE, url,
        URL_SIZE, MAX_URL_SIZE, &minor_version, &request_len);

    if (parse_err == PARSE_ERROR) {
        printf("[ERROR] Error occured during http parse\n");
        close(*client_sockfd);
        free(client_sockfd);
        return NULL;
    }

    err = check_request(method, minor_version);
    if (err == ERROR_REQUEST_UNSUPPORT) {
        printf("[ERROR] Request is unsupported\n");
        close(*client_sockfd);
        free(client_sockfd);
        return NULL;
    }

    pthread_rwlock_wrlock(&cache_lock);
    HashValue* value = hashmap_get(cache, &(HashValue){.url = url});
    CacheEntry* entry;

    if (value == NULL) {
        entry = cache_entry_create(url, BUFFER_SIZE);

        value = &(HashValue){.url = url, .entry = entry};

        if (cache_size == MAX_CACHE_SIZE) {
            CacheEntry* removed_entry = cache_entry_remove(&queue_head);
            hashmap_delete(cache, &(CacheEntry){.url = removed_entry->url});

            int arc = __sync_fetch_and_sub(&removed_entry->arc, 1);
            if (arc == 1) {
                cache_entry_free(removed_entry);
            }

            cache_size--;
        }

        hashmap_set(cache, value);
        cache_size++;

        cache_entry_add(&queue_head, entry);

        __sync_fetch_and_add(&entry->arc, 2);

        RemoteServer* remote_data =
            create_remote_data(entry, request_len, url, client_request);

        pthread_t tid;
        pthread_create(&tid, NULL, handle_remote_server, (void*)remote_data);
        pthread_detach(tid);
    } else {
        entry = value->entry;
        __sync_fetch_and_add(&entry->arc, 1);
        cache_entry_upd(&queue_head, entry);
        printf("[INFO] Reading entry from cache %p", entry);
    }
    pthread_rwlock_unlock(&cache_lock);

    size_t written = 0;
    size_t offset = entry->response_len - entry->response_len % BUFFER_SIZE;
    List* node = entry->data;
    int parts_read = 0;
    int amount_to_read = 0;

    pthread_mutex_lock(&entry->wait_lock);
    while (entry->parts_done == 0 && !entry->done) {
        pthread_cond_wait(&entry->new_part, &entry->wait_lock);
    }
    pthread_mutex_unlock(&entry->wait_lock);

    while (node != NULL) {
        amount_to_read = node->buf_len;
        err = write(*client_sockfd, node->buffer + written,
                    amount_to_read - written);
        if (err == -1) {
            printf("[ERROR] Error during writing response to client: %s\n",
                   strerror(errno));
            __sync_fetch_and_sub(&entry->arc, 1);
            close(*client_sockfd);
            free(client_sockfd);
            return NULL;
        }
        written += err;

        if (written >= amount_to_read) {
            parts_read++;
            written = 0;

            if (entry->done) {
                node = node->next;
            } else {
                pthread_mutex_lock(&entry->wait_lock);
                while (parts_read == entry->parts_done && !entry->done) {
                    pthread_cond_wait(&entry->new_part, &entry->wait_lock);
                }
                node = node->next;
                pthread_mutex_unlock(&entry->wait_lock);
            }
        }
    }

    // Client thread exits, so it won't have a ref to entry
    int arc = __sync_fetch_and_sub(&entry->arc, 1);
    if (arc == 1) {
        cache_entry_free(entry);
    }

    close(*client_sockfd);
    free(client_sockfd);
    return NULL;
}

int main() {
    int err;

    int server_sockfd = create_server_socket_and_listen(PORT);
    if (server_sockfd == -1) {
        printf("[ERROR] Error starting proxy server\n");
        return -1;
    }

    cache = cache_create();
    cache_size = 0;
    queue_head = NULL;

    /*signal(SIGPIPE, broken_pipe_handler);*/
    signal(SIGPIPE, SIG_IGN);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len;

        int* client_sockfd = (int*)malloc(sizeof(int));
        *client_sockfd =
            accept(server_sockfd, (struct sockaddr*)&client_addr, &client_len);

        if (*client_sockfd < 0) {
            if (errno == EINTR) {
                continue;
            }

            printf("Unable to accept");

            continue;
        }

        printf("New client\n");

        pthread_t client_thread;
        pthread_create(&client_thread, NULL, handle_client, client_sockfd);
        pthread_detach(client_thread);
    }

    return 0;
}
