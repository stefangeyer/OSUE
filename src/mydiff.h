/**
 * @file mydiff.h
 * @author Stefan Geyer <stefan.geyer@student..tuwien.ac.at>
 * @date 16.10.2017
 *
 * @brief Provides difference function for main
 *
 * The util module. It contains generic functions related to standard in and output.
 *
 */

#ifndef MYDIFF_H /* prevent multiple inclusion */
#define MYDIFF_H
#define LINE_MAX 100 /**< Lines ready by this program should not be longer than the specified amount */
#include <stdio.h>
#include <stdbool.h>

/**
 * Find difference between file contents.
 * @brief Prints all line numbers that don't match the other file's line
 * and the amount of differing characters per line.
 * @details This function only compares lines up to LINE_MAX characters.
 * @param file1 File used for comparison
 * @param file2 File used for comparison
 * @param ignore_case If true, capital letters will be handled as non caps.
 */
void difference(FILE *file1, FILE *file2, bool ignore_case);

#endif // MYDIFF_H
