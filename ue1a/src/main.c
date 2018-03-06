/**
 * @file main.c
 * @author Stefan Geyer <stefan.geyer@student.tuwien.ac.at>
 * @date 06.03.2018
 *
 * @brief Main program module.
 *
 * This program compresses the given input and prints out a summary afterwards.
 **/

#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "compress.h"
#include "util.h"
#include "main.h"

#define LINE_LENGTH 100 /**< Lines ready by this program should not be longer than the specified amount */

char *pgm_name; /**< The program name.*/
static FILE *output_file = NULL; /**< Output file handle. Must be global to use it with atexit*/
static int _read, _written; /**< Read and written counter. Global for easier access*/

void error_exit(char *format, ...) {
    va_list arg;
    char f[1024];
    snprintf(f, 1024, "%s: %s\n", pgm_name, format);

    va_start(arg, format);
    vfprintf(stderr, f, arg);
    va_end(arg);

    exit(EXIT_FAILURE);
}

/**
 * @brief Clean up function that frees or closes allocated resources
 * @details Should be called with atexit
 */
static void clean_up(void) {
    if (output_file != NULL) {
        fclose(output_file);
    }
}

/**
 * Mandatory usage function.
 * @brief This function writes helpful usage information about the program to stderr.
 * @details global variables: pgm_name
 */
static void usage(void) {
    fprintf(stderr, "SYNOPSIS\n\t%s [-o outfile] [infile1] [infile2] ...\n", pgm_name);
    exit(EXIT_FAILURE);
}

/**
 * Transforms the given input.
 * @brief Transforms the given string to it's compressed form and prints it
 * @details Input may at most be ilen chars long
 * @param input The input string
 * @param ilen The input length
 */
static void transform(char *input, size_t ilen) {
    size_t olen = 2 * ilen;
    char output[olen];

    _read += ilen;
    _written += strcmpr(input, output, olen);

    fprintf(output_file, "%s", output);
}

/**
 * Program entry point.
 * @brief This function takes care about parameters and initiates the compression
 * @details The main function performs argument parsing using the getopt function. Duplicate options without arguments
 * are not allowed.
 * global variables: pgm_name
 * @param argc The argument counter.
 * @param argv The argument vector.
 * @return Returns EXIT_SUCCESS or exists via fopen_error.
 */
int main(int argc, char *argv[]) {
    pgm_name = argv[0];

    if (atexit(clean_up) != 0) {
        error_exit("Cannot define cleanup function");
    }

    int c, opt_o = 0;
    float pct;
    bool invopt = false;

    output_file = stdout;

    // 3rd param: optstring defines legitimate option characters.
    // Appending : to an argument character implies, that the option requires an argument
    while ((c = getopt(argc, argv, "o:")) != -1) {
        switch (c) {
            case 'o':
                // Option o is optional and may occur once.
                opt_o++;
                output_file = fopen(optarg, "w");
                if (output_file == NULL) {
                    error_exit("Cannot open output file: %s", optarg);
                }
                break;
            case '?':
                invopt = true;
                break;
            default:
                assert(0);
        }
    }

    // Check whether all options were supplied correctly or not
    if (opt_o > 1 || invopt) usage();

    if ((argc - optind) > 0) {
        for (int i = 0; i < (argc - optind); i++) for_each_line(argv[optind + i], LINE_LENGTH, transform);
    } else {
        char input[LINE_LENGTH];

        printf("Please enter some text: ");
        fgets(input, LINE_LENGTH, stdin);

        transform(input, strlen(input));
    }

    pct = (float) _written * 100 / _read;

    fprintf(stderr, "Gelesen:\t\t %d Zeichen\n"
            "Geschrieben:\t\t %d Zeichen\n"
            "Komprimierungsrate:\t %.2f %%\n", _read, _written, pct);

    return EXIT_SUCCESS;
}