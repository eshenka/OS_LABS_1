#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 8192
#define MAX_URL_SIZE 256
#define MAX_CACHE_SIZE 1024
#define PORT 1080
#define MAX_METHOD_SIZE 4

void* handle_client(void* arg) {
    int* client_sockfd = (int*)arg;

    char client_request[BUFFER_SIZE];
    ssize_t size_read = read(*client_sockfd, client_request, BUFFER_SIZE);
    client_request[size_read] = '\0';

    char method[MAX_METHOD_SIZE];
    int method_i = 0;
    while (client_request[method_i] != ' ') {
        method[method_i] = client_request[method_i];
        method_i++;
    }
    method[MAX_METHOD_SIZE - 1] = '\0';

    if (strcmp(method, "GET")) {
        printf("Disconnected. Only GET method is available\n");
        close(*client_sockfd);
        return NULL;
    }

    int url_i = 4; // {GET / HTTP/1.0}
    char url[MAX_URL_SIZE];
    while (client_request[url_i] != ' ') {
        url[url_i] = client_request[url_i];
        url_i++;
    }
    url[url_i] = '\0';

    int version_i = url_i + 1 + 5; // one for space and 5 for HTTP/
    char version[4];
    strncpy(version, client_request + version_i, 3);
    version[3] = '\0';

    if (strcmp(version, "1.0")) {
        printf("Disconnected. Only 1.0 version is supported\n");
        close(*client_sockfd);
        return NULL;
    }

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
        int client_sockfd =
            accept(server_sockfd, (struct sockaddr*)&client_addr, &client_len);

        if (client_sockfd < 0) {
            if (errno == EINTR) {
                continue;
            }

            printf("Unable to accept");

            continue;
        }

        printf("new client\n");

        pthread_t client_thread;
        pthread_create(&client_thread, NULL, handle_client, &client_sockfd);
        pthread_detach(client_thread);
    }

    return 0;
}
