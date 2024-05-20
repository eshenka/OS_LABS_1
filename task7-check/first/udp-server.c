#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 10000
#define BUFFER_SIZE 256

int main() {
    int socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);

    if (socket_fd == -1) {
        perror("Unable to create socket\n");
        exit(-1);
    }

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));


}
