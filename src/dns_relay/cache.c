#include <stdio.h>
#include <stdlib.h>
#include "cache.h"

void init_cache() {
    cache_node = create_node();
    cache_cnt = 0;
}

void record_dn(char url[], uint8_t* buf, size_t buf_len) {
    insert(cache_node, url, buf, buf_len);
    cache_cnt++;
    printf("Adding a new record: %s\n", url);
}

struct TIP* get_ip_from_cache(char url[]) {
    struct Trie* p = search_prefix(cache_node, url);
    if (p && p->is_end) {
        return p->tip;
    }

    return NULL;
}

void clear_cache() {
    free_tree(cache_node);
    cache_node = NULL;
    cache_cnt = 0;
}