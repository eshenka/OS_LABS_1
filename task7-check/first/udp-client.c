#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>

int main() {
    const int PORT = 6969;

    int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (socket_fd == -1) {
        perror("Unable to create socket\n");
        exit(-1);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    const int BUFFER_SIZE = 256;
    char buffer[BUFFER_SIZE];

    socklen_t addrlen;
    
    while(1) {

        char* input = fgets(buffer, BUFFER_SIZE, stdin);
        if (input == NULL) {
            perror("Unable to get input");
            exit(-1);
        }

        int sent = sendto(socket_fd, buffer, strlen(buffer), 0, (struct sockaddr *) &server_addr, sizeof(server_addr));

        if (sent == -1) {
            perror("Unable to send\n");
            continue;
        }


        ssize_t recieved = recvfrom(socket_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &server_addr, &addrlen);

        if (recieved == -1) {
            perror("Unable to recieve\n");
            break;
        }

        printf("%s\n", buffer);

    }

    close(socket_fd);

    return 0;
}
