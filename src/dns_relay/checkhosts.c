#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "checkhosts.h"
#include "../common/utils.h"

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
            if (strcmp(dn[i], "0.0.0.0")) {
                message->rcode = NameError_ResponseCode;
                printf("Invalid Domain: %s\n", dn[i]);
            }
            else {
                message->anCount++;
                message->nsCount++;
            }
        }
    }
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
    struct ResourceRecord* rp = malloc(sizeof(struct ResourceRecord));
    while (q) {
        if (look_in_table(message, q, rp) || look_in_cache(message, q, rp)) return 0;
        q = q->next;
        
    }

    return 1; // Continue -> DNS Relay
}