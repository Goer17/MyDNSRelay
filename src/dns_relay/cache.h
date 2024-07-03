#if !defined(CACHE_H)
#define CACHE_H

#include "../common/trie.h"

Trie* cache_node;
static int cache_cnt;

void init_cache();

struct TIP* get_ip_from_cache(char url[]);

// Record new IP address
void record_dn(char url[], char ip_addr[]);

// Clear all cache
void clear_cache();

#endif // DEBUG