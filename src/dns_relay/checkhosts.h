#if !defined(CHECKHOSTS_H)
#define CHECKHOSTS_H

#include "parser.h"

#define MAX_LEN 128
#define MAX_MAP_NUM 1024

static char dn[MAX_MAP_NUM][MAX_LEN];
static char ip[MAX_MAP_NUM][MAX_LEN];
static int dn_cnt = 0;

void load_map();
int check_hosts(struct Message* message);

#endif // CHECKHOSTS_H
