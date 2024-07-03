#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

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
            if (strcmp(ip[i], "0.0.0.0") == 0) {
                message->rcode = NameError_ResponseCode;
                printf("Invalid Domain!\n");
                return 0; // Stop
            }
            else {
                strcpy(message->questions->qName, ip[i]);
                printf("Found in local DB.\n");
                return 0; // Stop
            }
        }
    }

    return 1; // Continue -> DNS Relay
}