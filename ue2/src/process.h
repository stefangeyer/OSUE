/**
 * @file process.h
 * @author Stefan Geyer <e1625718 at student.tuwien.ac.at>
 * @date 16.05.2018
 *
 * @brief Subprocess header.
 *
 * This header declares the subprocess functions that are required for this task and a custom char array structure.
 **/

#ifndef UE2_PROCESS_H
#define UE2_PROCESS_H

#include "main.h"

/**
 * Interprocess mergesort variant
 *
 * @brief Sorts the given lines array using a mergesort variant.
 * @details Instead of normal recursion, a recursive child process is called
 *          Global variables: pgm_name
 *
 * @param amount The amount of lines
 * @param lines The lines to sort
 */
void forksort(long int amount, char lines[][LINE_LENGTH]);

#endif //UE2_PROCESS_H
