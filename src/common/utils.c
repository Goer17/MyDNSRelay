#include <stdlib.h>
#include "util.h"

void error_handling(char *message) {
    perror(message);
    exit(1);
}