#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>

int main() {
    const int PORT = 6969;

    int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (socket_fd == -1) {
        perror("Unable to create socket\n");
        exit(-1);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    int bind_status = bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (bind_status != 0) {
        perror("Unable to bind");
        exit(-1);
    }

    struct sockaddr client_addr;
    socklen_t addrlen;

    int listen_status = listen(socket_fd, 10);
    if (listen_status == -1) {
        perror("Unable to listen");
        exit(-1);
    }

    while (1) {
        int client_addr = accept(socket_fd, NULL, NULL);

        if (client_addr == -1) {
            perror("Unable to accept");
            exit(-1);
        }

        pid_t new_connection = fork();
        if (new_connection == 0) {
            const int BUFFER_SIZE = 256;
            char buffer[BUFFER_SIZE];
                
            while(1) {
                ssize_t recieved = recv(client_addr, buffer, BUFFER_SIZE, 0);
                if (recieved == -1) {
                    perror("Unable to recieve");
                    exit(-1);
                }

                if (recieved == 0) {
                    close(client_addr);
                    exit(0);
                }

                write(STDOUT_FILENO, buffer, recieved);

                ssize_t sent = send(client_addr, buffer, recieved, 0);

                if (sent == -1) {
                    perror("Unable to send");
                    exit(-1);
                }
            }

            exit(0);
        }
        
    }

    close(socket_fd);  

    return 0;
}
