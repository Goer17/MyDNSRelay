#ifndef TRIE_H
#define TRIE_H

#define TRIE_LEN 95
#define C_TO_X(x) (x - 32)
#define TIP_BUF_LEN 512

struct TIP {
    size_t buf_len;
    char buf[TIP_BUF_LEN];
};

struct Trie {
    struct Trie* ch[TRIE_LEN];
    int is_end;
    struct TIP* tip;
};

struct Trie* create_node();
struct Trie* search_prefix(struct Trie* p, char prefix[]);
void insert(struct Trie* p, char url[], char* buf, size_t buf_len);
int search(struct Trie* p, char url[]);

void free_tree(struct Trie* p);

#endif // !TRIE_H
