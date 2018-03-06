#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include "compress.h"
#include "util.h"
#include "main.h"

#define LINE_LENGTH 100 /**< Lines ready by this program should not be longer than the specified amount */

char *pgm_name;
static FILE *output_file = NULL;
static int _read, _written;

void error_exit(char *format, ...) {
    va_list arg;
    char f[1024];
    snprintf(f, 1024, "%s: %s\n", pgm_name, format);

    va_start(arg, format);
    vfprintf(stderr, f, arg);
    va_end(arg);

    exit(EXIT_FAILURE);
}

static void clean_up(void) {
    if (output_file != NULL) {
        fclose(output_file);
    }
}

static void usage(void) {
    fprintf(stderr, "SYNOPSIS\n\t%s [-o outfile] [infile1] [infile2] ...\n", pgm_name);
    exit(EXIT_FAILURE);
}

static void transform(char *input, size_t ilen) {
    size_t olen = 2 * ilen;
    char output[olen];

    _read += ilen;
    _written += strcmpr(input, output, olen);

    fprintf(output_file, "%s", output);
}

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