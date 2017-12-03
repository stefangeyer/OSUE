#include "main.h"
#include "process.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <getopt.h>
#include <sys/wait.h>
#include <string.h>

static char *pgm_name; /**< program name */
static char *directory;
static char *prefix = "";

// variables that need to be freed before exiting
static char_array_t *array;
static char *md5, *fi;

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