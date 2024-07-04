#if !defined(CHECKHOSTS_H)
#define CHECKHOSTS_H

#include "parser.h"

#define MAX_LEN 128
#define MAX_MAP_NUM 1024

static char dn[MAX_MAP_NUM][MAX_LEN];
static char ip[MAX_MAP_NUM][MAX_LEN];
static int dn_cnt = 0;

enum {
    DN_INVALID,
    DN_FOUND_IN_TABLE,
    DN_NOT_IN_TABLE
};

enum {
    DN_IN_CACHE,
    DN_NOT_IN_CACHE
};

void load_map();
int look_in_table(struct Message* message);
struct TIP* look_in_cache(struct Message* message);

#endif // CHECKHOSTS_H
