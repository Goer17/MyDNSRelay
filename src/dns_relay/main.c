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
    uint8_t buf[BUF_SIZE];
    ssize_t num_bytes;

    relay_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (relay_sock == -1) {
        error_handling("socket() error");
    }

    // Local address
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(LOCAL_PORT);

    if (bind(relay_sock, (struct sockaddr*)&local_addr, sizeof(local_addr)) == -1) {
        error_handling("bind() error");
    }

    // DNS address
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
            error_handling("recvfrom client");
        }
        
        struct Message message;
        memset(&message, 0, sizeof(message));
        if (!decode_msg(&message, buf)) {
            error_handling("decoding client requirement");
        }
        
        printf("Received one package from %s:\n", inet_ntoa(client_addr.sin_addr));
        print_message(&message);

        uint16_t client_id = message.id;
        char msg_dn[128];
        strcpy(msg_dn, message.questions->qName);

        // Search in table
        int flag_t = look_in_table(&message);
        if (flag_t == DN_INVALID || flag_t == DN_FOUND_IN_TABLE) {
            uint8_t* p_buf = buf;
            printf("Found in table, send to client:\n");
            print_message(&message);
            if (encode_msg(&message, &p_buf)) {
                error_handling("encoding the message to client (in table)");
            }
            num_bytes = p_buf - buf;
            if (sendto(relay_sock, buf, num_bytes, 0, (struct sockaddr*)&client_addr, client_addr_size) == -1) {
                error_handling("sendto the client (in table)");
            }
            continue;
        }

        // Search in cache
        struct TIP* tip = look_in_cache(&message);
        if (tip) {
            printf("Found in cache...\n");
            num_bytes = tip->buf_len;
            for (int i = 0; i < num_bytes; i++) buf[i] = tip->buf[i];
            uint16_t* bbuf = (uint16_t*)buf;
            bbuf[0] = htons(client_id);

            if (sendto(relay_sock, buf, num_bytes, 0, (struct sockaddr*)&client_addr, client_addr_size) == -1) {
                error_handling("sendto the client (in table)");
            }
            continue;
        }

        // Send requirement to DNS server if necessary
        if (sendto(relay_sock, buf, num_bytes, 0, (struct sockaddr*)&dns_addr, sizeof(dns_addr)) == -1) {
            error_handling("sendto DNS");
        }

        // Received response from DNS server
        num_bytes = recvfrom(relay_sock, buf, BUF_SIZE, 0, NULL, NULL);
        if (num_bytes == -1) {
            error_handling("recvfrom DNS");
        }

        memset(&message, 0, sizeof(message));
        if (!decode_msg(&message, buf)) {
            error_handling("decoding DNS response");
        }
        printf("Received the response from DNS server:\n");
        print_message(&message);
        
        record_dn(msg_dn, buf, num_bytes);

        // Send the response to client
        if (sendto(relay_sock, buf, num_bytes, 0, (struct sockaddr*)&client_addr, client_addr_size) == -1) {
            error_handling("sendto");
        }
    }

    close(relay_sock);
    return 0;
}
