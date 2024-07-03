#include <stdlib.h>
#include "cache.h"

void init_cache() {
    cache_node = create_node();
    cache_cnt = 0;
}

void record_dn(char url[], char ip_addr[]) {
    insert(cache_node, url, ip_addr);
    cache_cnt++;
}

struct TIP* get_ip_from_cache(char url[]) {
    return get_ip(cache_node, url);
}

void clear_cache() {
    free_tree(cache_node);
    cache_node = NULL;
    cache_cnt = 0;
}