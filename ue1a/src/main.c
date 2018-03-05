#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "main.h"
#include "compress.h"
#include "util.h"

static char *pgm_name;
static FILE *output_file = NULL;

int main(int argc, char *argv[]) {
    pgm_name = argv[0];

    if (atexit(clean_up) != 0) {
        error_exit("Cannot define cleanup function");
    }

    int c, opt_o = 0, pos_args = argc - optind, read = 0, written = 0;
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
                if (output_file == NULL) error_exit("Cannot open output file");
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

    if (pos_args > 0) {
        for (int i = 0; i < pos_args; i++) {
            process_input(argv[optind + i], &read, &written);
        }
    } else {
        process_input(NULL, &read, &written);
    }

    pct = (float) written * 100 / read;

    fprintf(stderr, "Gelesen:\t\t\t %d Zeichen\n"
            "Geschrieben:\t\t %d Zeichen\n"
            "Komprimierungsrate:\t %.2f %%\n", read, written, pct);

    return EXIT_SUCCESS;
}

static void process_input(char *file_name, int *read, int *written) {
    char line[LINE_LENGTH], res[LINE_LENGTH * 2];
    int len;

    if (file_name != NULL) {
        FILE *fp = fopen(file_name, "r");
        if (fp == NULL) error_exit("Cannot open input file");

        while ((len = get_line(fp, line, LINE_LENGTH)) > 0) {
            *read += len;
            *written += strcmpr(line, res, sizeof(res));
            fprintf(stderr, "stderr: %s", res);
            printf("stdout: %s", res);
        }

        fclose(fp);
    } else {

    }
}

static void clean_up() {
    if (output_file != NULL) {
        fclose(output_file);
    }
}

void error_exit(char *reason) {
    fprintf(stderr, "%s: %s\n", pgm_name, reason);
    exit(EXIT_FAILURE);
}

void usage(void) {
    fprintf(stderr, "SYNOPSIS\n\t%s [-o outfile] [infile1] [infile2] ...\n", pgm_name);
    exit(EXIT_FAILURE);
}