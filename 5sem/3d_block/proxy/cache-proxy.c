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

    char host[URL_SIZE], path[URL_SIZE];
    sscanf(url, "http://%[^/]%s", host, path);

    struct hostent* server;
    server = gethostbyname(host);

    int server_sockfd = connect_to_remote_server(server);
    if (server_sockfd == -1) {
        printf("Unable to connect to a remote server\n");
        close(*client_sockfd);
        free(client_sockfd);
        return NULL;
    }

    size_t written = 0;
    while (written < request_len) {
        written += write(server_sockfd, client_request + written,
                         request_len - written);
    }

    List* response = create_list(BUFFER_SIZE);
    size_t response_len = 0;

    parse_err = parse_http_response(server_sockfd, response, BUFFER_SIZE,
                                    &response_len);
    if (parse_err == PARSE_ERROR) {
        close(*client_sockfd);
        free(client_sockfd);
        return NULL;
    }

    written = 0;
    size_t offset = response_len - response_len % BUFFER_SIZE;
    List* head = response;

    while (response != NULL) {
        int d = 0;
        if (response->next == 0) {
            d = offset;
        }

        err = write(*client_sockfd, response->buffer + written,
                    BUFFER_SIZE - d - written);
        written += err;

        if (written >= BUFFER_SIZE) {
            response = response->next;
            written = 0;
        }
    }

    free_list(head);
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
