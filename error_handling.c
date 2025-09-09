// error_handling.c
#include <stdio.h>
#include <stdlib.h>
#include "error_handling.h"

void display_error(const char *message) {
    fprintf(stderr, "Error: %s\n", message);
    return;
}
