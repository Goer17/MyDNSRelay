#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

void error_handling(char *message) {
    perror(message);
    exit(1);
}