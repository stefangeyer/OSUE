#include <string.h>
#include "util.h"

bool has_prefix(const char *pre, const char *str) {
    return strlen(pre) > 0 && strncmp(pre, str, strlen(pre)) == 0;
}