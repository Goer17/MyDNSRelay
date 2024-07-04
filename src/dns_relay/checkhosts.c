#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "checkhosts.h"
#include "../common/utils.h"
#include "cache.h"

int ipv4_string_to_uint8(const char *ipv4_str, uint8_t *add) {
    struct in_addr addr;
    uint8_t ip[4];
    if (inet_pton(AF_INET, ipv4_str, &addr) != 1) {
        printf("change false.\n");
        return -1; // 转换失败
    }
    uint32_t ip_addr = ntohl(addr.s_addr);
    ip[0] = (ip_addr >> 24) & 0xFF;
    add[0] = ip[0];
    ip[1] = (ip_addr >> 16) & 0xFF;
    add[1] = ip[1];
    ip[2] = (ip_addr >> 8) & 0xFF;
    add[2] = ip[2];
    ip[3] = ip_addr & 0xFF;
    add[3] = ip[3];
    return 0; // 转换成功
}

void load_map() {
    int fd = open("../knownhosts.txt", O_RDONLY);
    if (fd == -1) {
        error_handling("open");
    }
    if (dup2(fd, STDIN_FILENO) == -1) {
        close(fd);
        error_handling("dup2");
    }
    printf("Loading...\n");
    while (scanf("%s %s", ip[dn_cnt], dn[dn_cnt]) != EOF) {
        // printf("%s %s\n", ip[dn_cnt], dn[dn_cnt]);
        dn_cnt++;
        if (dn_cnt >= MAX_MAP_NUM) break;
    }
    close(fd);
}

int look_in_table(struct Message* message) {
    struct Question* q = message->questions; // 只有一个请求
    for (int i = 0; i < dn_cnt; i++) {
        if (strcmp(q->qName, dn[i]) == 0) {
            printf("Found in table...\n");
            message->qr = 1;
            message->aa = 1;
            if (strcmp(ip[i], "0.0.0.0") == 0) {
                message->rcode = NameError_ResponseCode;
                printf("Invalid Domain: %s\n", dn[i]);
                return DN_INVALID;
            }
            else {
                message->answers = malloc(sizeof(struct ResourceRecord));
                struct ResourceRecord* rp = message->answers;
                rp->next = NULL;
                rp->cls = q->qClass;
                rp->type = q->qType;
                rp->ttl = 1800;
                rp->rd_length = 4; // Table 里面只有 4
                rp->name = q->qName; // ?
                message->anCount++;
                ipv4_string_to_uint8(ip[i], rp->rd_data.a_record.addr);
                return DN_FOUND_IN_TABLE;
            }
        }
    }
    return DN_NOT_IN_TABLE;
}

struct TIP* look_in_cache(struct Message* message) {

    return get_ip_from_cache(message->questions->qName);
}
