#if !defined(LOGGER_H)
#define LOGGER_H
#include <stdio.h>

#define STD_LOG(cmd)    \
    do {                \
        to_log();       \
        cmd;            \
        fflush(stdout); \
        to_stdout();    \
        cmd;            \
    } while (0)

static int log_fd;
static int saved_stdout_fd;
void open_log();
void write_time_to_log();
void to_log();
void to_stdout();

#endif