#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "network.h"

#define ERROR -1

int create_server_socket_and_listen(int port) {
    int err;

    int server_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_sockfd < 0) {
        return ERROR;
    }

    int one = 1;
    err =
        setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    if (err) {
        close(server_sockfd);
        return ERROR;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    err = bind(server_sockfd, (struct sockaddr*)&server_addr,
               sizeof(server_addr));
    if (err) {
        close(server_sockfd);
        return ERROR;
    }
    listen(server_sockfd, 5);

    printf("Proxy server listening on port %d\n", port);
    return server_sockfd;
}

int connect_to_remote_server(struct hostent* server) {
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_sockfd < 0) {
        return ERROR;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    server_addr.sin_port = htons(80);

    int con = connect(server_sockfd, (struct sockaddr*)&server_addr,
                      sizeof(server_addr));
    if (con != 0) {
        close(server_sockfd);
        return ERROR;
    }

    return server_sockfd;
}
