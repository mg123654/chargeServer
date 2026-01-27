#ifndef NETWORK_H
#define NETWORK_H

int network_init(const char *ip, int port);
int network_accept(int server_fd);
int network_send(int client_fd, const char *data);
int network_recv(int client_fd, char *buffer, int size);
void network_close(int fd);

#endif
