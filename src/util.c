#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "util.h"

bool has_prefix(const char *pre, const char *str) {
    return strncmp(pre, str, strlen(pre)) == 0;
}