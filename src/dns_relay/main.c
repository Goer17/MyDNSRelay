#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "../common/utils.h"

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    struct Request packet;
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);
    
    // 绑定套接字到地址
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    for ( ;; ) {
        // 接收数据
        int n = recvfrom(sockfd, &packet, sizeof(packet), MSG_WAITALL, (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) {
            perror("recvfrom failed");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        
        printf("Received packet: ID=%d, Domain=%s\n", packet.id, packet.dn);
        pid_t pid = fork();
        if (pid == 0) {
            execlp("nslookup", "nslookup", packet.dn, "8.8.8.8", NULL);
            perror("execlp");
            exit(EXIT_FAILURE);
        }
    }
    
    // close(sockfd);
    return 0;
}
