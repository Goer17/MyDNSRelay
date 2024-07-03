#ifndef PARSER_H
#define PARSER_H

#define ID_MASK (((1 << 15) - 1) << 15)
#define R_CODE_MASK (0xf)

int id;
int r_code;

void parse(void* buffer);

#endif // !PARSER_H
