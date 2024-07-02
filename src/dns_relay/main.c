#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 512
#define DNS_SERVER_IP "26.26.26.53"
#define DNS_SERVER_PORT 53
#define LOCAL_PORT 53

void die(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    int sockfd;
    struct sockaddr_in local_addr, remote_addr, client_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE];
    ssize_t received_size;

    // UDP 套接字
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        die("socket creation failed");
    }

    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    local_addr.sin_port = htons(LOCAL_PORT);

    if (bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        die("bind failed");
    }

    for ( ;; ) {
        // 接收客户端请求
        received_size = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
        if (received_size < 0) {
            die("recvfrom failed");
        }

        // 转发请求到远程 DNS 服务器
        memset(&remote_addr, 0, sizeof(remote_addr));
        remote_addr.sin_family = AF_INET;
        remote_addr.sin_addr.s_addr = inet_addr(DNS_SERVER_IP);
        remote_addr.sin_port = htons(DNS_SERVER_PORT);

        if (sendto(sockfd, buffer, received_size, 0, (struct sockaddr *)&remote_addr, sizeof(remote_addr)) < 0) {
            // TODO
            die("sendto failed");
        }

        // 接收 DNS 服务器的响应
        received_size = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
        if (received_size < 0) {
            die("recvfrom failed");
        }

        // 将响应发送回客户端
        if (sendto(sockfd, buffer, received_size, 0, (struct sockaddr *)&client_addr, addr_len) < 0) {
            die("sendto failed");
        }
    }

    close(sockfd);
    return 0;
}
