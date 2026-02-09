#include "network.h"
#include "protocol.h"
#include "session.h"
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

// 主程序入口
// 初始化服务器，监听客户端连接，处理充电协议
int main() {
    // 注册信号处理器（Ctrl+C 和 SIGTERM 优雅关闭）
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // 打印启动信息
    printf("========================================\n");
    printf("   Charging Station Server v1.0\n");
    printf("========================================\n");
    printf("Binding to: 0.0.0.0 (all interfaces)\n");
    printf("Listening on port: %d\n", PORT);
    printf("========================================\n\n");

    // 初始化TCP服务器（绑定0.0.0.0监听所有网络接口）
    int server_fd = network_init(NULL, PORT);
    if (server_fd < 0) {
        fprintf(stderr, "Failed to initialize network\n");
        return 1;
    }

    printf("Server bound to 0.0.0.0:%d, waiting for connections...\n\n", PORT);

    // 主循环：持续接受客户端连接
    while (running) {
        // 等待客户端连接
        int client_fd = network_accept(server_fd);
        if (client_fd < 0) {
            if (running) {
                continue;
            } else {
                break;
            }
        }
        
        // 为每个客户端创建会话（记录登录状态）
        Session *session = session_create();
        if (session == NULL) {
            fprintf(stderr, "Failed to create session\n");
            network_close(client_fd);
            continue;
        }

        // 创建接收缓冲区和响应缓冲区
        char buffer[BUFFER_SIZE];
        char response[BUFFER_SIZE];

        printf("Session started\n\n");

        // 客户端处理循环：接收并处理每个命令
        while (running) {
            // 从客户端接收数据
            int bytes_read = network_recv(client_fd, buffer, BUFFER_SIZE);

            // 客户端断开连接
            if (bytes_read <= 0) {
                printf("Client disconnected\n\n");
                break;
            }

            // 按行解析命令（以\r\n分隔）
            char *line = strtok(buffer, "\r\n");
            while (line != NULL && strlen(line) > 0) {
                printf("Received: %s\n", line);

                // 根据协议规范处理命令并生成响应
                protocol_process(line, session, response, sizeof(response));

                // 如果有响应数据，发送给客户端
                if (strlen(response) > 0) {
                    network_send(client_fd, response);
                    printf("Sent: %s", response);
                }

                // 处理下一行命令
                line = strtok(NULL, "\r\n");
            }

            printf("\n");
        }

        // 清理会话和客户端连接
        session_destroy(session);
        network_close(client_fd);
    }

    // 关闭服务器socket
    network_close(server_fd);

    printf("\nServer stopped\n");
    return 0;
}
