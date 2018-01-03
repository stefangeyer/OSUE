#include <stdbool.h>
#include <assert.h>
#include "server.h"
#include "../common/common.h"

static void usage(void);
void parse_arguments(int argc, char *argv[]);

static char *pgm_name;
static char *db_file_name = NULL;
static node_t *head = NULL;

int main(int argc, char *argv[]) {
    // This also handles setting pgm_name
    parse_arguments(argc, argv);

    if (db_file_name != NULL) head = read_node(db_file_name);

//    authshm_t *shared = memory_create();


//    memory_destroy(shared);

    node_t *n1 = create_node("a", "b", "c"), *n2 = create_node("d", "ax", "hi");

    head = append_node(head, n1);
    head = append_node(head, n2);

    print_node(head);

    write_node(head, "auth-server.db.csv");

    destroy_node(head);

    return EXIT_SUCCESS;
}

void parse_arguments(int argc, char *argv[]) {
    pgm_name = argv[0];

    int c;
    int opt_l = 0;
    bool invopt = false;

    // 3rd param: optstring defines legitimate option characters.
    // Appending : to an argument character implies, that the option requires an argument
    while ((c = getopt(argc, argv, "l:")) != -1) {
        switch (c) {
            case 'l':
                // Option l is optional and may occur once.
                opt_l++;
                db_file_name = optarg;
                break;
            case '?':
                invopt = true;
                break;
            default:
                assert(0);
        }
    }

    if (opt_l > 1) {
        // error_exit("Database option can only be set once.", true);
        // error
    }

    // Check whether there are the correct amount of positional arguments or an invalid option were supplied.
    if ((argc - optind) != 0 || invopt) usage();
}

/**
 * Mandatory usage function.
 * @brief This function writes helpful usage information about the program to stderr.
 * @details global variables: pgm_name
 */
static void usage(void) {
    fprintf(stderr, "SYNOPSIS\n\t%s [-l database]\n", pgm_name);
    exit(EXIT_FAILURE);
}