#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>

#define PORT 6969
#define BUFFER_SIZE 256

int main() {
    int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (socket_fd == -1) {
        perror("Unable to create socket\n");
        exit(-1);
    }

    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    int bind_status = bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (bind_status != 0) {
        perror("Unable to bind");
        exit(-1);
    }

    while(1) {
        char buffer[BUFFER_SIZE];

        struct sockaddr client_socket;

        socklen_t addrlen;
    
        ssize_t recieved = recvfrom(socket_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &client_socket, &addrlen);

        if (recieved == -1) {
            perror("Unable to recieve");
            continue;
            //exit(-1);
        }

        write(STDOUT_FILENO, buffer, recieved);

        ssize_t sent = sendto(socket_fd, buffer, recieved, 0, (struct sockaddr *) &client_socket, addrlen);

        if (sent == -1) {
            perror("Unable to send\n");
            //exit(-1);
        }
    }

    close(socket_fd);
}
