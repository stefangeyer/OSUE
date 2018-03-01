/**
 * @file util.h
 * @author Stefan Geyer <e1625718 at student.tuwien.ac.at>
 * @date 03.12.2017
 *
 * @brief Utility header.
 *
 * This header defines the prototype for all utility functions used in this task
 **/

#include <stdbool.h>

#ifndef UE2_UTIL_H

/**
 * Checks if a string starts with a certain prefix
 *
 * @brief Tests whether a given string starts with a certain prefix. If the prefix is empty, false is returned.
 * @param pre The prefix
 * @param str The string to check
 * @return true if pre is not empty and str starts with pre, false otherwise
 */
bool has_prefix(const char *pre, const char *str);

#define UE2_UTIL_H

#endif //UE2_UTIL_H
