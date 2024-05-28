#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <string.h>

#define PORT 10000
#define BUFFER_SIZE 256

int main() {
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (socket_fd == -1) {
        perror("Unable to create socket\n");
        exit(-1);
    }

    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = PORT;
    server_addr.sin_addr.s_addr = INADDR_LOOPBACK;

    int bind_status = bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (bind_status != 0) {
        perror("Unable to bind");
        exit(-1);
    }

    while(1) {
        char buffer[BUFFER_SIZE];

        struct sockaddr_in client_socket;

        ssize_t recieved = recvfrom(socket_fd, &buffer, BUFFER_SIZE, 0, (struct sockaddr *) &client_socket, NULL);

        printf("%s\n", buffer);

        sendto(socket_fd, buffer, recieved, 0, (struct sockaddr *) &client_socket, NULL);
    }
}
