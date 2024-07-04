#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "utils.h"
#include "logger.h"

void open_log() {
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);

    char buffer[32];
    strftime(buffer, sizeof(buffer), "../logs/%Y-%m-%d.log", local);
    log_fd = open(buffer, O_RDWR | O_APPEND | O_CREAT, 0644);
    if (log_fd == -1) {
        error_handling("open");
    }
}

void write_log(char* txt, int len) {
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);

    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", local);
    char time_buf[128];
    sprintf(time_buf, "----%s----\n", buffer);

    if (write(log_fd, time_buf, strlen(time_buf)) == -1) { // 写入正确的长度
        error_handling("write_time");
    }
    if (write(log_fd, txt, len) == -1) {
        error_handling("write_txt");
    }
}