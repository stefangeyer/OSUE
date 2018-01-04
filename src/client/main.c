#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "main.h"

static char *pgm_name, *username, *password, session[SESSION_LENGTH];
static int mode;

int main(int argc, char *argv[]) {
    // This also handles setting pgm_name
    parse_arguments(argc, argv);

    if (mode == MODE_LOGIN) {
        if (login(username, password, session) < 0) {
            // error
        }

        printf("Login was successful.\n");
        interact();
    } else if (mode == MODE_REGISTER) {
        if (signup(username, password) < 0) {
            // error
        }
        printf("Signup was successful.\n");
    }


    return EXIT_SUCCESS;
}

static void interact(void) {
    printf("Commands:\n"
                   "\t1) write secret\n"
                   "\t2) read secret\n"
                   "\t3) logout\n");

    bool go = true;
    while (go) {
        printf("Please select a command (1-3): ");

        char input[3], secret[SECRET_LENGTH];
        fgets(input, 3, stdin);

        switch (input[0]) {
            case '1':
                memset(&secret[0], 0, sizeof(secret));

                printf("Please enter the new secret: ");
                fgets(secret, SECRET_LENGTH, stdin);

                if (write_secret(session, secret) < 0) {
                    // error
                }

                printf("Secret was successfully updated!\n");
                break;
            case '2':
                memset(&secret[0], 0, sizeof(secret));

                if (read_secret(session, secret) < 0) {
                    // error
                }

                printf("%s\n", secret);

                break;
            case '3':
                if (logout(session) < 0) {
                    // error
                }
                go = false;
                printf("Bye.\n");
                break;
            default:
                break;
        }
    }
}

static void parse_arguments(int argc, char *argv[]) {
    pgm_name = argv[0];

    int c, opt_l = 0, opt_r = 0;
    bool invopt = false;

    // 3rd param: optstring defines legitimate option characters.
    // Appending : to an argument character implies, that the option requires an argument
    while ((c = getopt(argc, argv, "lr")) != -1) {
        switch (c) {
            case 'l':
                // Option l XOR r are required and may occur once.
                opt_l++;
                break;
            case 'r':
                // Option l XOR r are required and may occur once.
                opt_r++;
                break;
            case '?':
                invopt = true;
                break;
            default:
                assert(0);
        }
    }

    if ((opt_r + opt_l) == 0 || (opt_r + opt_l) > 1) {
        // error Either option l or r must occur, but not both of them
    }

    mode = (opt_r == 1) ? MODE_REGISTER : MODE_LOGIN;

    // Check whether there are the correct amount of positional arguments (un, pw) or an invalid option were supplied.
    if ((argc - optind) != 2 || invopt) usage();

    username = argv[optind];
    password = argv[optind + 1];
}

/**
 * Mandatory usage function.
 * @brief This function writes helpful usage information about the program to stderr.
 * @details global variables: pgm_name
 */
static void usage(void) {
    fprintf(stderr, "SYNOPSIS\n\t%s { -r | -l } username password\n", pgm_name);
    exit(EXIT_FAILURE);
}