#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int network_init(const char *ip, int port) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_fd);
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (ip == NULL || strlen(ip) == 0) {
        server_addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
            fprintf(stderr, "Invalid IP address: %s\n", ip);
            close(server_fd);
            return -1;
        }
    }

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 1) < 0) {
        perror("listen");
        close(server_fd);
        return -1;
    }

    return server_fd;
}

int network_accept(int server_fd) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd < 0) {
        perror("accept");
        return -1;
    }

    printf("Client connected from %s:%d\n",
           inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));

    return client_fd;
}

int network_send(int client_fd, const char *data) {
    if (client_fd < 0 || data == NULL) {
        return -1;
    }

    int len = strlen(data);
    int sent = send(client_fd, data, len, 0);

    if (sent < 0) {
        perror("send");
        return -1;
    }

    return sent;
}

int network_recv(int client_fd, char *buffer, int size) {
    if (client_fd < 0 || buffer == NULL || size <= 0) {
        return -1;
    }

    int bytes_read = recv(client_fd, buffer, size - 1, 0);

    if (bytes_read < 0) {
        perror("recv");
        return -1;
    }

    if (bytes_read == 0) {
        return 0;
    }

    buffer[bytes_read] = '\0';
    return bytes_read;
}

void network_close(int fd) {
    if (fd >= 0) {
        close(fd);
    }
}
