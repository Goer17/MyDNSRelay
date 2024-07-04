#if !defined(LOGGER_H)
#define LOGGER_H

static int log_fd;
void open_log();
void write_log(char* txt, int len);

#endif