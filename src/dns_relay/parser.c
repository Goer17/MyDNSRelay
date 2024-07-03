#include "parser.h"

void parse(void* buffer) {
    int* p = (int*)buffer;
    r_code = p[0] & R_CODE_MASK;

}