#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "../common/utils.h"
#include "client.h"

#define PORT 8080
#define BUFFER_SIZE 1024

int main(int argc, char const *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    for ( ;; ) {
        struct Request request;
        printf("> ");
        scanf("%s", request.dn);
        if (strcmp(request.dn, ":q") == 0 || strcmp(request.dn, ":quit") == 0) break;

        memset(&server_addr, 0, sizeof(server_addr));
    
        // 设置 DNS Relay 地址
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(8080);

        if (sendto(sockfd, (void *)&request, sizeof(request), 0, (struct sockarr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("sendto failed");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
    }

    close(sockfd);
    
    return 0;
}
