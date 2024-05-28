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
    
    while(1) {
        char buffer[BUFFER_SIZE];

        char* helpme = fgets(buffer, BUFFER_SIZE, stdin);
        if (helpme == NULL) {
            perror("ya riday");
            exit(-1);
        }

        struct sockaddr_in client_socket;
        client_socket.sin_family = AF_INET;

        int sent = sendto(socket_fd, buffer, strlen(buffer), 0, (struct sockaddr *) &server_addr, sizeof(server_addr));

        if (sent == -1) {
            printf("Unable to send\n");
            continue;
        }

        socklen_t addrlen;

        ssize_t recieved = recvfrom(socket_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &server_addr, &addrlen);

        if (recieved == -1) {
            printf("Unable to recieve\n");
            break;
        }

        printf("%s\n", buffer);

    }

    close(socket_fd);
}
