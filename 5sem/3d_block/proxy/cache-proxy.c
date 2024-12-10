#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <assert.h>

#include "picohttpparser/picohttpparser.h"

#define BUFFER_SIZE 8192
#define MAX_URL_SIZE 256
#define MAX_CACHE_SIZE 1024
#define PORT 1080
#define MAX_METHOD_SIZE 4

void* handle_client(void* arg) {
    int* client_sockfd = (int*)arg;

    /*char client_request[BUFFER_SIZE];*/
    /*ssize_t size_read = read(*client_sockfd, client_request, BUFFER_SIZE);*/
    /*client_request[size_read] = '\0';*/
    /**/
    /*char method[MAX_METHOD_SIZE];*/
    /*int method_i = 0;*/
    /*while (client_request[method_i] != ' ') {*/
    /*    method[method_i] = client_request[method_i];*/
    /*    method_i++;*/
    /*}*/
    /*method[MAX_METHOD_SIZE - 1] = '\0';*/
    /**/
    /*if (strcmp(method, "GET")) {*/
    /*    printf("Disconnected. Only GET method is available\n");*/
    /*    close(*client_sockfd);*/
    /*    free(client_sockfd);*/
    /*    return NULL;*/
    /*}*/
    /**/
    /*int url_i = 4; // {GET / HTTP/1.0}*/
    /*char url[MAX_URL_SIZE];*/
    /*while (client_request[url_i] != ' ') {*/
    /*    url[url_i] = client_request[url_i];*/
    /*    url_i++;*/
    /*}*/
    /*url[url_i] = '\0';*/
    /**/
    /*int version_i = url_i + 1 + 5; // one for space and 5 for HTTP/*/
    /*char version[4];*/
    /*strncpy(version, client_request + version_i, 3);*/
    /*version[3] = '\0';*/
    /**/
    /*if (strcmp(version, "1.0")) {*/
    /*    printf("Disconnected. Only 1.0 version is supported\n");*/
    /*    close(*client_sockfd);*/
    /*    free(client_sockfd);*/
    /*    return NULL;*/
    /*}*/

    char client_request[BUFFER_SIZE];
    const char* method;
    const char* url;
    struct phr_header headers[100];

    size_t buflen = 0;
    size_t prevbuflen = 0;
    ssize_t rret;

    size_t num_headers;
    size_t method_len;
    size_t url_len;

    int pret, minor_version;

    while (1) {
        while ((rret = read(*client_sockfd, client_request + buflen,
                            sizeof(client_request) - buflen) == -1 &&
                       errno == EINTR))
            ;
        if (rret <= 0) {
            close(*client_sockfd);
            free(client_sockfd);
            return NULL;
        }

        prevbuflen = buflen;
        buflen += rret;

        num_headers = sizeof(headers) / sizeof(headers[0]);
        pret = phr_parse_request(client_request, buflen, &method, &method_len,
                                 &url, &url_len, &minor_version, headers,
                                 &num_headers, prevbuflen);

        if (pret > 0) {
            break;
        } else if (pret == -1) {
            close(*client_sockfd);
            free(client_sockfd);
            return NULL;
        }

        assert(pret == -2);

        if (buflen == sizeof(client_request)) {
            printf("Closing connection. Request is too long\n");
            close(*client_sockfd);
            free(client_sockfd);
            return NULL;
        }

        printf("Request is successfuly parsed!\n");
        printf("It is\n");
        printf("method: %s\n", method);
        printf("url: %s\n", url);
        printf("version 1.%d\n", minor_version);
        printf("headers:\n");

        for (int i = 0; i != num_headers; i++) {
            printf("%.*s: %.*s\n", (int)headers[i].name_len, headers[i].name,
                   (int)headers[i].value_len, headers[i].value);
        }
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

        printf("new client\n");

        pthread_t client_thread;
        pthread_create(&client_thread, NULL, handle_client, client_sockfd);
        pthread_detach(client_thread);
    }

    return 0;
}
