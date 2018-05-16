/**
 * @file main.h
 * @author Stefan Geyer <e1625718 at student.tuwien.ac.at>
 * @date 03.12.2017
 *
 * @brief Main program header
 *
 * This header defines the prototype for functions that may be used from other modules than the main module.
 **/

#ifndef UE2_MAIN_H
#define UE2_MAIN_H

#define LINE_LENGTH (62)

extern char *pgm_name; /**< Program name */

/**
 * Mandatory usage function.
 *
 * @brief This function writes helpful usage information about the program to stderr and exits via error_exit(NULL).
 * @details global variables: pgm_name
 */
void usage(void);

/**
 * Prints an error message to stderr and exits with failure.
 *
 * @brief This function prints an error message to stderr and exits with EXIT_FAILURE.
 * @details The provided error message will be appended to the program name before printing.
 * global variables: pgm_name
 * @param error The message to print; If error is NULL or empty, the program exits without printing
 */
void error_exit(char *message);

/**
 * Read line to the given pointer.
 *
 * @brief Reads a line from stdin including newline
 * @details Allocates the references pointer. Must be feed manually.
 *
 * @param lineptr
 */
void read_line(char **lineptr);

#endif //UE2_MAIN_H
