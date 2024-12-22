#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
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
#define MAX_BUFFER_SIZE 8192 * 8
#define URL_SIZE 1024
#define MAX_URL_SIZE 1024 * 8
#define MAX_CACHE_SIZE 1024
#define PORT 2367
#define METHOD_SIZE 4

struct hashmap* cache;
pthread_rwlock_t cache_lock = PTHREAD_RWLOCK_INITIALIZER;

typedef struct RemoteServer {
    char* url;
    CacheEntry* entry;
    size_t request_len;
    char* client_request;
} RemoteServer;

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

    char host[URL_SIZE], path[URL_SIZE];
    sscanf(url, "http://%[^/]%s", host, path);

    struct hostent* server;
    server = gethostbyname(host);

    int server_sockfd = connect_to_remote_server(server);
    if (server_sockfd == -1) {
        printf("Unable to connect to a remote server\n");
        /*close(*client_sockfd);*/
        /*free(client_sockfd);*/
        return NULL;
    }

    size_t written = 0;
    while (written < request_len) {
        written += write(server_sockfd, client_request + written,
                         request_len - written);
    }

    size_t response_len = 0;

    HTTP_PARSE parse_err =
        parse_http_response(server_sockfd, BUFFER_SIZE, &response_len, entry);
    if (parse_err == PARSE_ERROR) {
        /*close(*client_sockfd);*/
        /*free(client_sockfd);*/
        return NULL;
    }

    entry->response_len = response_len;
    /*entry->done = true;*/

    return NULL;
}

void* handle_client(void* arg) {
    Error err;

    int* client_sockfd = (int*)arg;

    size_t request_len = 0;
    char client_request[BUFFER_SIZE];
    char method[METHOD_SIZE];
    char url[URL_SIZE];
    int minor_version;

    HTTP_PARSE parse_err;
    parse_err = parse_http_request(*client_sockfd, client_request, BUFFER_SIZE,
                                   method, METHOD_SIZE, url, URL_SIZE,
                                   &minor_version, &request_len);

    if (parse_err == PARSE_ERROR) {
        printf("Error occured during http parse\n");
        close(*client_sockfd);
        free(client_sockfd);
        return NULL;
    }

    err = check_request(method, minor_version);
    if (err == ERROR_REQUEST_UNSUPPORT) {
        printf("Request is unsupported\n");
        close(*client_sockfd);
        free(client_sockfd);
        return NULL;
    }

    CacheEntry* entry;
    pthread_rwlock_wrlock(&cache_lock);
    entry = hashmap_get(cache, &(CacheEntry){.url = url});
    List* response = create_list(BUFFER_SIZE);

    if (entry == NULL) {
        hashmap_set(cache, &(CacheEntry){.arc = 0,
                                         .url = url,
                                         .lock = PTHREAD_RWLOCK_INITIALIZER,
                                         .data = create_list(BUFFER_SIZE),
                                         .response_len = 0,
                                         .done = false,
                                         .parts_done = 0});
        entry = hashmap_get(cache, &(CacheEntry){.url = url});

        RemoteServer* remote_data = (RemoteServer*)malloc(sizeof(RemoteServer));
        remote_data->entry = entry;
        remote_data->request_len = request_len;
        remote_data->client_request = client_request;
        remote_data->url = url;

        pthread_t tid;
        pthread_create(&tid, NULL, handle_remote_server, (void*)remote_data);
        pthread_detach(tid);
    }
    pthread_rwlock_unlock(&cache_lock);

    while (!entry->done) {
        usleep(100);
    }

    size_t written = 0;
    size_t offset = entry->response_len - entry->response_len % BUFFER_SIZE;
    List* node = entry->data;
    int parts_read = 0;
    int amount_to_read = 0;

    printf("\nstart\n");

    while (node != NULL) {
        amount_to_read = node->buf_len;
        err = write(*client_sockfd, node->buffer + written,
                    amount_to_read - written);
        written += err;
        printf("%lu\n", amount_to_read - written);

        if (written >= amount_to_read) {
            parts_read++;
            node = node->next;
            written = 0;
        }
    }
    printf("\nend\n");

    printf("\nentry = %s\n", entry->url);

    /*free_list(head);*/
    close(*client_sockfd);
    free(client_sockfd);
    return NULL;
}

int main() {
    int err;

    int server_sockfd = create_server_socket_and_listen(PORT);
    if (server_sockfd == -1) {
        printf("Error starting proxy server\n");
        return -1;
    }

    cache = create_cache();

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
