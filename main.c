#include "network.h"
#include "protocol.h"
#include "session.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define PORT 9002
#define BUFFER_SIZE 1024

volatile int running = 1;

void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        printf("\nServer shutting down...\n");
        running = 0;
    }
}

int main() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    char public_ip[64];
    utils_get_public_ip(public_ip, sizeof(public_ip));

    printf("========================================\n");
    printf("   Charging Station Server v1.0\n");
    printf("========================================\n");
    printf("Public IP: %s\n", public_ip);
    printf("Listening on port: %d\n", PORT);
    printf("========================================\n\n");

    int server_fd = network_init(public_ip, PORT);
    if (server_fd < 0) {
        fprintf(stderr, "Failed to initialize network\n");
        return 1;
    }

    printf("Server bound to %s:%d, waiting for connections...\n\n", public_ip, PORT);

    while (running) {
        int client_fd = network_accept(server_fd);
        if (client_fd < 0) {
            if (running) {
                continue;
            } else {
                break;
            }
        }

        Session *session = session_create();
        if (session == NULL) {
            fprintf(stderr, "Failed to create session\n");
            network_close(client_fd);
            continue;
        }

        char buffer[BUFFER_SIZE];
        char response[BUFFER_SIZE];

        printf("Session started\n\n");

        while (running) {
            int bytes_read = network_recv(client_fd, buffer, BUFFER_SIZE);

            if (bytes_read <= 0) {
                printf("Client disconnected\n\n");
                break;
            }

            char *line = strtok(buffer, "\r\n");
            while (line != NULL && strlen(line) > 0) {
                printf("Received: %s\n", line);

                protocol_process(line, session, response, sizeof(response));

                if (strlen(response) > 0) {
                    network_send(client_fd, response);
                    printf("Sent: %s", response);
                }

                line = strtok(NULL, "\r\n");
            }

            printf("\n");
        }

        session_destroy(session);
        network_close(client_fd);
    }

    network_close(server_fd);

    printf("\nServer stopped\n");
    return 0;
}
