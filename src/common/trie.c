#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "trie.h"

struct Trie* create_node() {
    struct Trie* node = (struct Trie*)malloc(sizeof(struct Trie));
    node->is_end = 0;
    memset(node->ch, 0, sizeof(node->ch));
    node->tip = NULL;

    return node;
}

struct Trie* search_prefix(struct Trie* p, char prefix[]) {
    int i = 0;
    while (prefix[i]) {
        int x = C_TO_X(prefix[i]);
        p = p->ch[x];
        if (p == NULL) return NULL;
        i++;
    }

    return p;
}

void insert(struct Trie* p, char url[], uint8_t* buf, size_t buf_len) {
    int i = 0;
    while (url[i]) {
        int x = C_TO_X(url[i]);
        if (p->ch[x] == NULL) {
            p->ch[x] = create_node();
        }
        p = p->ch[x];
        i++;
    }
    p->is_end = 1;
    p->tip = malloc(sizeof(struct TIP));
    for (int i = 0; i < buf_len; i++) {
        p->tip->buf[i] = buf[i];
    }
    p->tip->buf_len = buf_len;
}

int search(struct Trie* p, char url[]) {
    p = search_prefix(p, url);
    return p && p->is_end;
}

void free_tree(struct Trie* p) {
    if (p == NULL) return;
    for (int i = 0; i < TRIE_LEN; i++) {
        free_tree(p->ch[i]);
        p->ch[i] = NULL;
    }
    if (p->tip) free(p->tip);
    free(p);
}