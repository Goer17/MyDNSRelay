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

void write_time_to_log() {
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);

    char buffer[64];
    strftime(buffer, sizeof(buffer), "----%Y-%m-%d %H:%M:%S----\n", local);
    if (write(log_fd, buffer, strlen(buffer)) == -1) {
        error_handling("write");
    }
}

void to_log() {
    saved_stdout_fd = dup(STDOUT_FILENO);
    if (saved_stdout_fd == -1) {
        error_handling("dup");
    }

    if (dup2(log_fd, STDOUT_FILENO) == -1) {
        error_handling("dup2");
    }
}

void to_stdout() {
    if (dup2(saved_stdout_fd, STDOUT_FILENO) == -1) {
        error_handling("dup2");
    }

    close(saved_stdout_fd);
}
