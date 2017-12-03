/**
 * @file main.c
 * @author Stefan Geyer <e1625718 at student.tuwien.ac.at>
 * @date 03.12.2017
 *
 * @brief Main program module.
 *
 * This program gets the content of a directory and prints out the name, md5sum and info of every file.
 * Directories are ignored.
 **/

#include "main.h"
#include "process.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <getopt.h>
#include <sys/wait.h>
#include <string.h>

// PROTOTYPE

/**
 * Takes argc and argv from the main function and sets required properties
 *
 * @brief Parses options and makes sure there is one positional argument
 * @details Parses directory and checks whether the is a prefix to ignore or not
 * @param argc arg count
 * @param argv arg vector
 */
void parse_arguments(int argc, char *argv[]);

/**
 * Frees allocated resources
 *
 * @brief Frees the pointers stored in the global variables array, md5 and fi
 * @details global variables: array, md5, fi
 */
void clean_up(void);

// GLOBAL VARIABLES

static char *pgm_name; /**< Program name */
static char *directory; /**< The directory to scan */
static char *prefix = ""; /**< Prefix of ignored files */

// variables that need to be freed before exiting
static char_array_t *array; /**< The retrieved directory content */
static char *md5; /**< The current md5sum result */
static char *fi; /**< The current file info result */

// FUNCTIONS

/**
 * Program entry point.
 * @brief This function takes care about parameters, and calls md5sum and file_info for each file in the directory
 * @details If a file starts with the given prefix, it is ignored. When md5 is NULL, the current file is a directory and is ignored.
 * global variables: array, md5, fi, prefix
 * @param argc The argument counter.
 * @param argv The argument vector.
 * @return Returns EXIT_SUCCESS or exits via a called function
 */
int main(int argc, char *argv[]) {
    parse_arguments(argc, argv); // this also sets pgm_name

    array = list_directory(directory);

    for (int i = 0; i < array->size; i++) {
        char *file = array->array[i];
        if (!has_prefix(prefix, file)) {
            char *md5 = md5sum(directory, file);
            char *fi = file_info(directory, file);
            if (md5 != NULL){
                printf("%s %s %s\n", file, md5, fi);
            }
            free(md5);
            free(fi);
        }
    }

    clean_up();

    return EXIT_SUCCESS;
}

void usage(void) {
    fprintf(stderr, "SYNOPSIS:\n\t%s [-i ignorepraefix] <directory>\n", pgm_name);
    error_exit(NULL);
}

void clean_up() {
    free(md5);
    free(fi);
    free_array(array);
}

void error_exit(char *error) {
    if (error != NULL && strlen(error) > 0) fprintf(stderr, "%s: %s\n", pgm_name, error);
    clean_up();
    exit(EXIT_FAILURE);
}

void parse_arguments(int argc, char *argv[]) {
    pgm_name = argv[0];

    struct option long_options[] = {
            {"ignorepraefix", no_argument, 0, 'i'}
    };

    int c, opt_idx, opt_i = 0;
    bool inv_opt = false;

    // 3rd param: optstring defines legitimate option characters.
    // Appending : to an argument character implicates that the option requires an argument
    while ((c = getopt_long(argc, argv, "i:", long_options, &opt_idx)) != -1) {
        switch (c) {
            case 0:
                // If this option set a flag, do nothing else now.
                if (long_options[opt_idx].flag != 0)
                    break;
            case 'i':
                // Option i is optional and may occur once.
                if (opt_i != 0)
                    usage();
                prefix = optarg;
                opt_i = 1;
                break;
            case '?':
                inv_opt = true;
                break;
            default:
                assert(0);
        }
    }

    // Check whether there are the correct amount of positional argument or an invalid option were supplied.
    if ((argc - optind) != 1 || inv_opt) usage();

    directory = argv[optind];
}