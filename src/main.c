#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <getopt.h>

static char *pgm_name; /**< program name */
static char *directory;
static bool ignore_prefix = false;

void usage(void) {
    fprintf(stderr, "SYNOPSIS:\n\t%s [-i ignorepraefix] <directory>", pgm_name);
    exit(EXIT_FAILURE);
}

void parse_arguments(int argc, char *argv[]) {
    pgm_name = argv[0];

    struct option long_options[] = {
            {"ignorepraefix", no_argument, 0, 'i'}
    };

    int c;
    int opt_idx = 0;
    bool has_invalid_option = false;

    // 3rd param: optstring defines legitimate option characters.
    // Appending : to an argument character implicates that the option requires an argument
    while ((c = getopt_long(argc, argv, "i", long_options, &opt_idx)) != -1) {
        switch (c) {
            case 0:
                // If this option set a flag, do nothing else now.
                if (long_options[opt_idx].flag != 0)
                    break;
            case 'i':
                // Option i is optional and may occur more than once.
                ignore_prefix = true;
                break;
            case '?':
                has_invalid_option = true;
                break;
            default:
                assert(0);
        }
    }

    // Check whether there are the correct amount of positional argument or an invalid option were supplied.
    if ((argc - optind) != 1 || has_invalid_option) usage();

    directory = argv[optind];
}

int main(int argc, char *argv[]) {
    // this also sets pgm_name
    parse_arguments(argc, argv);

    return EXIT_SUCCESS;
}