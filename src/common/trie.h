#ifndef TRIE_H
#define TRIE_H

#define TRIE_LEN 95
#define C_TO_X(x) (x - 32)
#define IP_LEN 128

struct TIP {
    char ip_addr[IP_LEN];
};

struct Trie {
    struct Trie* ch[TRIE_LEN];
    int is_end;
    struct TIP* tip;
};

struct Trie* create_node();
struct Trie* search_prefix(struct Trie* p, char prefix[]);
void insert(struct Trie* p, char url[], char ip_addr[]);
int search(struct Trie* p, char url[]);
struct TIP* get_ip(struct Trie* p, char url[]);
void free_tree(struct Trie* p);

#endif // !TRIE_H
