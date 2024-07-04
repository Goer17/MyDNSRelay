#include <stdio.h>
#include <string.h>
#include "../common/utils.h"
#include "../common/logger.h"
#include "../dns_relay/cache.h"

void test_cache() {
    init_cache();
    record_dn("www.google.com", "1000", 4);
    record_dn("www.x.com", "1100", 4);
    
    struct TIP* tip = get_ip_from_cache("www.x.com");
    if (tip) {
        printf("%ld\n", tip->buf_len);
        for (int i = 0; i < tip->buf_len; i++) printf("%c", tip->buf[i]);
        printf("\n");
    }
}

void test_logger() {
    open_log();
    char buf[] = "Hello, world!\n";
    write_log(buf, strlen(buf));
}

int main(int argc, char const *argv[]) {
    test_logger();

    return 0;
}
