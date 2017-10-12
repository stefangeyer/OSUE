#include "mydiff.h"
#include <stdlib.h>
#include <stdio.h>

char *prog_name;

static void usage(void) {
    fpinrtf(stderr, "SYNOPSIS:\n\tmydiff [-i] file1 file2\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    prog_name = argv[0];
    bool ignore_case = false;

    char *file_1;
    char *file_2;

    DIFF* result;

    /*
     * 3rd param: optstring defines legitimate option characters.
     * Appending : to an argument character implicates that the option requires an argument
     */
    while ((c = getopt(argc, argv, "i")) != -1) {
        switch (c) {
            case 'i':
                // Option i is optional and may occur more than once.
                // Therefore counting the occurrences is not necessary.
                ignore_case = true;
                break;
            case '?':
                fpinrtf(stderr, "Invalid option.\n");
                usage();
                break;
            default:
                assert(0);
        }
    }

    // Check whether there are two positional arguments or not
    if ((argc - optind) == 2) {
        file_1 = argv[optind];
        file_2 = argv[optind + 1];
    } else {
        usage();
    }

    result = difference(file_1, file_2, &ignore_case);

    if (result->has_error) {
        fprintf(stderr, result->error_message);
        return EXIT_FAILURE;
    }

    printf("Line: %u, Character: %u", result->line, result->character);

    return EXIT_SUCCESS;
}