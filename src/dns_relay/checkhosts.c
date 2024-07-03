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

int check_hosts(struct Message* message) {
    for (int i = 0; i < dn_cnt; i++) {
        if (strcmp(message->questions->qName, dn[i]) == 0) {
            message->qr = 1;
            message->aa = 1;
            message->ra = 1;
            message->anCount = 0;
            message->nsCount = 0;
            message->arCount = 0;
            
            if (strcmp(ip[i], "0.0.0.0") == 0) {
                message->rcode = NameError_ResponseCode;
                printf("Invalid Domain!\n");
                return 0; // Stop
            }
            else {
                struct Question* p = message->questions;
                struct ResourceRecord* res = malloc(sizeof(struct ResourceRecord));
                struct ResourceRecord* rp = res;
                while (p) {
                    memset(res, 0, sizeof(res));
                    strcpy(rp->name, p->qName);
                    rp->type = p->qType;
                    rp->cls = p->qClass;
                    rp->ttl = 60 * 60;

                    rp->rd_length = 4;

                    // TODO
                    rp->next = malloc(sizeof(struct ResourceRecord));
                    rp = rp->next;
                    
                    p = p->next;
                }
                printf("Found in local DB.\n");
                return 0; // Stop
            }
        }
    }

    return 1; // Continue -> DNS Relay
}