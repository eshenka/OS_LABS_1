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

typedef enum Error { SUCCESS, ERROR_REQUEST_UNSUPPORT } Error;

#define BUFFER_SIZE 8192
#define URL_SIZE 256
#define MAX_CACHE_SIZE 1024
#define PORT 1080
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

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    char host[URL_SIZE], path[URL_SIZE];
    sscanf(url, "http://%[^/]%s", host, path);

    struct hostent* server;
    server = gethostbyname(host);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    server_addr.sin_port = htons(80);

    int con =
        connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (con != 0) {
        printf("Unable to connect to a remote server\n");
        close(*client_sockfd);
        free(client_sockfd);
        return NULL;
    }

    size_t written = 0;
    while (written < request_len) {
        written +=
            write(sockfd, client_request + written, request_len - written);
    }

    char response[BUFFER_SIZE];
    size_t response_len = 0;

    parse_err =
        parse_http_response(sockfd, response, BUFFER_SIZE, &response_len);
    if (parse_err == PARSE_ERROR) {
        close(*client_sockfd);
        free(client_sockfd);
        return NULL;
    }

    written = 0;
    while (written < response_len) {
        written +=
            write(*client_sockfd, response + written, response_len - written);
    }

    close(*client_sockfd);
    free(client_sockfd);
    return NULL;
}

int main() {
    int err;

    int server_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    int one = 1;
    err =
        setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    err = bind(server_sockfd, (struct sockaddr*)&server_addr,
               sizeof(server_addr));
    if (err == -1) {
        close(server_sockfd);
        printf("Error binding socket\n");
        printf("%s\b", errno);
        return -1;
    }
    listen(server_sockfd, 5);

    printf("Proxy server listening on port %d\n", PORT);

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
