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
#define MAX_CACHE_SIZE 1024
#define PORT 1000

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
    if (!err) {
        close(server_sockfd);
        printf("Error binding socket\n");
        printf("%s\b", errno);
        return -1;
    }
    listen(server_sockfd, 5);

    printf("Proxy server listening on port %d\n", PORT);

    while (1) {
        printf("waiting\n");
        struct sockaddr_in client_addr;
        socklen_t client_len;
        int client_sockfd =
            accept(server_sockfd, (struct sockaddr*)&client_addr, &client_len);

        if (client_sockfd < 0) {
            if (errno == EINTR) {
                printf("hello?\n");
                continue;
            }

            printf("Unable to accept");

            continue;
        }

        printf("new client\n");

        /*pthread_t client_thread;*/
        /*pthread_create(&client_thread, NULL, handle_client, (void
         * *)client_sockfd);*/
        /*pthread_detach(client_thread);*/
    }

    return 0;
}
