#ifndef NETWORK_H
#define NETWORK_H

int create_server_socket_and_listen(int port);

int connect_to_remote_server(struct hostent* server);

#endif // !NETWORK_H
