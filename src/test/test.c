#include <stdio.h>
#include <string.h>
#include "../common/utils.h"
#include "../common/logger.h"
#include "../dns_relay/cache.h"

void test_logger() {
    open_log();
    char buf[] = "Hello, world!";
    write_time_to_log();
    STD_LOG(puts(buf));
}

int main(int argc, char const *argv[]) {
    test_logger();
    
    return 0;
}
