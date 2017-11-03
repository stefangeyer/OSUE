/**
 * @file main.c
 * @author Stefan Geyer <stefan.geyer@student.tuwien.ac.at>
 * @date 16.10.2017
 *
 * @brief Main program module.
 *
 * This program compares the contents of two files. If a difference is found, the line number and the amount of
 * differing characters in said line are printed.
 **/

#include "mydiff.h"
#include <stdlib.h>
#include <string.h>
// Using the C99 standard on windows seems to prevent the POSIX macros from being defined (warnings) -> Include manually
#include <getopt.h>
#include <assert.h>
#include <errno.h>

static char *pgm_name; /**< The program name.*/

/**
 * Mandatory usage function.
 * @brief This function writes helpful usage information about the program to stderr.
 * @details global variables: pgm_name
 */
static void usage(void) {
    fprintf(stderr, "SYNOPSIS:\n\t%s [-i] file1 file2\n", pgm_name);
    exit(EXIT_FAILURE);
}

/**
 * Prints the given filename plus the corresponding error message to stderr. Should only be called, if fopen(2) == NULL
 * @brief This function writes fopen error messages to stderr and exits with EXIT_FAILURE.
 * @details global variables: pgm_name
 * @param filename The filename that lead to the error
 */
static void fopen_error(char *filename) {
    fprintf(stderr, "%s: Error opening file '%s': %s\n", pgm_name, filename, strerror(errno));
    exit(EXIT_FAILURE);
}

/**
 * Program entry point.
 * @brief This function takes care about parameters, opens files and passes the FILE handles to the file diff algorithm.
 * @details The main function performs argument parsing using the getopt function. Duplicate options without arguments
 * are allowed. File handles gathered by fopen are passed on to the difference function.
 * A file can't be compared with itself.
 * global variables: pgm_name
 * @param argc The argument counter.
 * @param argv The argument vector.
 * @return Returns EXIT_SUCCESS or exists via fopen_error.
 */
int main(int argc, char *argv[]) {
    pgm_name = argv[0];

    bool ignore_case = false;
    bool has_invalid_option = false;
    char *filename1, *filename2;
    FILE *file1, *file2;
    int c;

    // 3rd param: optstring defines legitimate option characters.
    // Appending : to an argument character implicates that the option requires an argument
    while ((c = getopt(argc, argv, "i")) != -1) {
        switch (c) {
            case 'i':
                // Option i is optional and may occur more than once.
                // Therefore counting the occurrences is not necessary.
                ignore_case = true;
                break;
            case '?':
                has_invalid_option = true;
                break;
            default:
                assert(0);
        }
    }

    // Check whether there are two positional arguments or not
    if ((argc - optind) != 2 || has_invalid_option) {
        usage();
    }

    filename1 = argv[optind];
    filename2 = argv[optind + 1];

    if (strcmp(filename1, filename2) == 0) {
        // No need to compare a file with itself
        fprintf(stderr, "Trying to compare a file with itself. Abort.");
        return EXIT_FAILURE;
    }

    file1 = fopen(filename1, "r");
    if (file1 == NULL) fopen_error(filename1);
    file2 = fopen(filename2, "r");
    if (file2 == NULL) fopen_error(filename2);

    difference(file1, file2, ignore_case);

    fclose(file1);
    fclose(file2);

    return EXIT_SUCCESS;
}