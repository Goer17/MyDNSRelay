#ifndef UTILS_H
#define UTILS_H

#define DOMAIN_MAXLEN 128
struct Request {
    int id;
    int opcode;
    int rcode;
    char dn[DOMAIN_MAXLEN];
};

#endif // !UTILS_H