#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "checkhosts.h"
#include "../common/utils.h"



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

int look_in_table(struct Message* message, struct Question* q, struct ResourceRecord* rp) {
    for (int i = 0; i < dn_cnt; i++) {
        if (strcmp(q->qName, dn[i]) == 0) {
            printf("found in table.\n");
            if (strcmp(ip[i], "0.0.0.0") == 0) {
                message->rcode = NameError_ResponseCode;
                printf("Invalid Domain: %s\n", dn[i]);
                return -1;
            }
            else {
                ipv4_string_to_uint8(ip[i],rp->rd_data.a_record.addr);
                return 1;
            }
        }
    }
    return 0;

}

int look_in_cache(struct Message* message, struct Question* q, struct ResourceRecord* rp) {
    //if finded return 1
    return 0;
}

int check_hosts(struct Message* message) {
    message->qr = 1;
    message->aa = 1;
    message->ra = 1;
    message->anCount = 0;
    message->nsCount = 0;
    message->arCount = 0;
    struct Question* q = message->questions;
    struct ResourceRecord* res = NULL;
    while (q) {

        res = malloc(sizeof(struct ResourceRecord));
        memset(res,0,sizeof(struct ResourceRecord));

        res->name = q->qName;
        res->type = q->qType;
        res->cls = q->qClass;
        res->ttl = 1800;
        res->rd_length = 4;
        int flag_t = look_in_table(message, q, res);
        int flag_c = 0;

        if (flag_t == -1) 
            return 0;
        if (flag_t == 0){
            flag_c = look_in_cache(message, q, res); 
            if (flag_c == 0 && flag_t == 0){
                printf("Not found in local.\n");
                free(res->name);
                free(res);
                return 1;
            }
        }
        message->anCount++;
        res->next = message->answers;
        message->answers = res;
        q = q->next;
    }
    return 0;
}
