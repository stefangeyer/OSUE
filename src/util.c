/**
 * @file util.c
 * @author Stefan Geyer <e1625718 at student.tuwien.ac.at>
 * @date 03.12.2017
 *
 * @brief Utility module.
 *
 * This module defines useful utility functions.
 **/

#include <string.h>
#include "util.h"

bool has_prefix(const char *pre, const char *str) {
    return strlen(pre) > 0 && strncmp(pre, str, strlen(pre)) == 0;
}