#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "../common/utils.h"
#include "parser.h"
#include "checkhosts.h"
#include "cache.h"

#define BUF_SIZE 512
#define LOCAL_PORT 53

#define GOOGLE_DNS "8.8.8.8"
#define DNS_PORT 53

int main(int argc, char *argv[]) {
    char dns_ip_addr[128] = GOOGLE_DNS;
    if (argc > 1) {
        strcpy(dns_ip_addr, argv[1]);
    }

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
    dns_addr.sin_addr.s_addr = inet_addr(dns_ip_addr);
    dns_addr.sin_port = htons(DNS_PORT);

    load_map();
    init_cache();
    printf("Relay Server Running...\n");
    for ( ;; ) {
        client_addr_size = sizeof(client_addr);
        num_bytes = recvfrom(relay_sock, buf, BUF_SIZE, 0, (struct sockaddr*)&client_addr, &client_addr_size);
        if (num_bytes == -1) {
            error_handling("recvfrom() error");
        }
        printf("Received one package from %s\n", inet_ntoa(client_addr.sin_addr));
        
        struct Message message;
        memset(&message, 0, sizeof(message));
        if (!decode_msg(&message, buf, num_bytes)) {
            error_handling("Decoding");
        }
        char msg_dn[128];
        strcpy(msg_dn, message.questions->qName); 
        print_message(&message);
        // printf("0\n");
        // printf("num_bytes: %d\n", num_bytes);
        int flag_c = check_hosts(&message);
        if (flag_c == 0) {
            uint8_t* buf_p = (uint8_t *)buf;
            if(!encode_msg(&message, &buf_p)) {
                error_handling("Encoding");
            }
            int buf_len = buf_p - (uint8_t*)buf;
            if (sendto(relay_sock, buf, buf_len, 0, (struct sockaddr*)&client_addr, client_addr_size) == -1) {
                error_handling("sendto() error");
            }
            continue;
        }

        if (flag_c == 2) {
            printf("Found in cache.\n");
            struct TIP* tip = get_ip_from_cache(message.questions->qName);
            strcpy(buf, tip->buf);
            int buf_len = tip->buf_len;
            // TODO 更改 buf ID
            if (sendto(relay_sock, buf, buf_len, 0, (struct sockaddr*)&client_addr, client_addr_size) == -1) {
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
        print_message(&message);
        record_dn(msg_dn, buf, num_bytes);

        if (sendto(relay_sock, buf, num_bytes, 0, (struct sockaddr*)&client_addr, client_addr_size) == -1) {
            error_handling("sendto() error");
        }
    }

    close(relay_sock);
    return 0;
}
