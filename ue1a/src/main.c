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

char *pgm_name; /**< The program name. */

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
 * Mandatory usage function.
 * @brief This function writes helpful usage information about the program to stderr.
 * @details global variables: pgm_name
 */
static void usage(void) {
    fprintf(stderr, "SYNOPSIS\n\t%s [-o outfile] [infile1] [infile2] ...\n", pgm_name);
    exit(EXIT_FAILURE);
}

static void perform_compress(char *src, int *read, int *written, FILE *output) {
    size_t clen = strlen(src);
    size_t rlen = clen * 2;
    char res[rlen];

    *read += clen;
    *written += strncmpr(src, res, rlen);

    fprintf(output, "%s\n", res);
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

    int c, opt_o = 0, pargs, read = 0, written = 0;
    float pct;
    bool invopt = false;
    FILE *output = stdout;

    // 3rd param: optstring defines legitimate option characters.
    // Appending : to an argument character implies, that the option requires an argument
    while ((c = getopt(argc, argv, "o:")) != -1) {
        switch (c) {
            case 'o':
                // Option o is optional and may occur once.
                opt_o++;
                output = fopen(optarg, "w");
                if (output == NULL) {
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

    pargs = argc - optind;

    if (pargs > 0) {
        for (int i = 0; i < pargs; i++) {
            char *input = read_file(argv[optind + i], LINE_LENGTH);
            perform_compress(input, &read, &written, output);
            free(input);
        }
    } else {
        char input[LINE_LENGTH];

        printf("Please enter some text: ");
        fgets(input, LINE_LENGTH, stdin);

        perform_compress(input, &read, &written, output);
    }

    pct = (float) written * 100 / read;

    fprintf(stderr, "Gelesen:\t\t %d Zeichen\n"
            "Geschrieben:\t\t %d Zeichen\n"
            "Kompressionsrate:\t %.2f %%\n", read, written, pct);

    fclose(output);

    return EXIT_SUCCESS;
}