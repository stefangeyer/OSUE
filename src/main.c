#include "mydiff.h"

static void usage(void) {
    // argv[0] also contains path to file -> easier to use static string
    fprintf(stderr, "SYNOPSIS:\n\tmydiff [-i] file1 file2\n");
    exit(EXIT_FAILURE);
}

static void fopen_error(char *filename) {
    fprintf(stderr, "Error opening file '%s': %s\n", filename, strerror(errno));
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    bool ignore_case = false;
    char *filename1, *filename2;
    FILE *file1, *file2;
    int c;

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
                fprintf(stderr, "Invalid option.\n");
                usage();
                break;
            default:
                assert(0);
        }
    }

    // Check whether there are two positional arguments or not
    if ((argc - optind) == 2) {
        filename1 = argv[optind];
        filename2 = argv[optind + 1];
    } else {
        usage();
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