#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "../common/utils.h"
#include "parser.h"
#include "checkhosts.h"

#define BUF_SIZE 512
#define LOCAL_PORT 53

#define GOOGLE_DNS "8.8.8.8"
#define GOOGLE_DNS_PORT 53

int main() {
    int relay_sock;
    struct sockaddr_in local_addr, dns_addr, client_addr;
    socklen_t client_addr_size;
    char buf[BUF_SIZE];
    ssize_t num_bytes;

    relay_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (relay_sock == -1) {
        error_handling("socket() error");
    }

    // 本地地址
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(LOCAL_PORT);

    if (bind(relay_sock, (struct sockaddr*)&local_addr, sizeof(local_addr)) == -1) {
        error_handling("bind() error");
    }

    memset(&dns_addr, 0, sizeof(dns_addr));
    dns_addr.sin_family = AF_INET;
    dns_addr.sin_addr.s_addr = inet_addr(GOOGLE_DNS);
    dns_addr.sin_port = htons(GOOGLE_DNS_PORT);

    load_map();
    printf("Relay Server Running...\n");
    for ( ;; ) {
        client_addr_size = sizeof(client_addr);
        num_bytes = recvfrom(relay_sock, buf, BUF_SIZE, 0, (struct sockaddr*)&client_addr, &client_addr_size);
        if (num_bytes == -1) {
            error_handling("recvfrom() error");
        }
        printf("Received one package from %s\n", inet_ntoa(client_addr.sin_addr));
        
        struct Message message;
        if (!decode_msg(&message, buf, num_bytes)) {
            error_handling("Decoding\n");
        }
        // print_message(&message);
        if (!check_hosts(&message)) {
            // Stop
            memset(buf, 0, sizeof(buf));
            if(!encode_msg(&message, buf)) {
                error_handling("Encoding");
            }
            if (sendto(relay_sock, buf, num_bytes, 0, (struct sockaddr*)&client_addr, client_addr_size) == -1) {
                error_handling("sendto() error");
            }
            continue;
        }

        if (sendto(relay_sock, buf, num_bytes, 0, (struct sockaddr*)&dns_addr, sizeof(dns_addr)) == -1) {
            error_handling("sendto() error");
        }

        num_bytes = recvfrom(relay_sock, buf, BUF_SIZE, 0, NULL, NULL);
        if (num_bytes == -1) {
            error_handling("recvfrom() error");
        }

        if (sendto(relay_sock, buf, num_bytes, 0, (struct sockaddr*)&client_addr, client_addr_size) == -1) {
            error_handling("sendto() error");
        }
    }

    close(relay_sock);
    return 0;
}
