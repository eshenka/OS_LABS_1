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

#include "picohttpparser/picohttpparser.h"

typedef enum Error { SUCCESS, ERROR_HTTP_PARSE, ERROR_REQUEST_UNSUPPORT } Error;

#define BUFFER_SIZE 8192
#define URL_SIZE 256
#define MAX_CACHE_SIZE 1024
#define PORT 1080
#define METHOD_SIZE 4

// Code in this function is using picohttpparser library. The way of parsing
// with it is fully described in README on github
Error parse_http(int client_sockfd, char* ret_client_request, char* ret_method,
                 char* ret_url, int* minor_version) {
    char client_request[BUFFER_SIZE];
    const char* method;
    const char* url;
    struct phr_header headers[100];
    size_t buflen = 0, prevbuflen = 0, num_headers, method_len, url_len;
    int pret;

    ssize_t rret;

    while (1) {
        while ((rret = read(client_sockfd, client_request + buflen,
                            sizeof(client_request) - buflen)) == -1 &&
               errno == EINTR)
            ;
        if (rret <= 0) {
            printf("Closing. {rret <= 0}\n");
            return ERROR_HTTP_PARSE;
        }

        prevbuflen = buflen;
        buflen += rret;

        num_headers = sizeof(headers) / sizeof(headers[0]);
        pret = phr_parse_request(client_request, buflen, &method, &method_len,
                                 &url, &url_len, minor_version, headers,
                                 &num_headers, prevbuflen);

        if (pret > 0) {
            break;
        } else if (pret == -1) {
            printf("Closing. {pret == -1}\n");
            return ERROR_HTTP_PARSE;
        }

        /*assert(pret == -2);*/
        if (pret != -2) {
            printf("Something went wrong during parsing http\n");
            return ERROR_HTTP_PARSE;
        }

        if (buflen == sizeof(client_request)) {
            printf("Closing connection. Request is too long\n");
            return ERROR_HTTP_PARSE;
        }
    }

    if (url_len > URL_SIZE) {
        printf("Closing. URL is too long\n");
        return ERROR_HTTP_PARSE;
    }

    strcpy(ret_client_request, client_request);
    ret_client_request[buflen - 1] = '\0';

    strncpy(ret_method, method, METHOD_SIZE - 1);
    ret_method[METHOD_SIZE - 1] = '\0';

    strcpy(ret_url, url);
    ret_url[url_len - 1] = '\0';

    return SUCCESS;
}

Error check_request(char* method, int minor_version) {
    if (!strcpy(method, "GET")) {
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

    char client_request[BUFFER_SIZE];
    char method[METHOD_SIZE];
    char url[URL_SIZE];
    int minor_version;

    err =
        parse_http(*client_sockfd, client_request, method, url, &minor_version);

    if (err == ERROR_HTTP_PARSE) {
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
    printf("%d\n", con);

    write(sockfd, client_request, strlen(client_request));

    char response[BUFFER_SIZE];

    printf("reading\n");

    int res = read(sockfd, response, BUFFER_SIZE);
    printf("%d %s\n", res, response);

    /*printf("Request is successfuly parsed!\n");*/
    /*printf("It's\n");*/
    /*printf("method: %s\n", method);*/
    /*printf("url: %s\n", url);*/
    /*printf("version 1.%d\n", minor_version);*/

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
