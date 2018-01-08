#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "main.h"

static char *pgm_name, username[USERNAME_LENGTH], password[PASSWORD_LENGTH], session[SESSION_LENGTH], secret[SECRET_LENGTH];
static auth_memory_t *shared;
static auth_semaphores_t *semaphores;
static int mode;

int main(int argc, char *argv[]) {
    // This also handles setting pgm_name
    parse_arguments(argc, argv);

    if (atexit(clean_up) != 0) {
        error_exit("Cannot define cleanup function");
    }

    shared = memory_open();
    semaphores = semaphores_open();

    int res;
    char error[80];

    if (mode == MODE_LOGIN) {
        if ((res = communicate(request_login, response_login)) < 0) {
            snprintf(error, sizeof(error), "Cannot perform login. (%d)", res);
            error_exit(error);
        }

        printf("Login was successful.\n");
        interact();
    } else if (mode == MODE_REGISTER) {
        if ((res = communicate(request_register, response_register)) < 0) {
            snprintf(error, sizeof(error), "Cannot cannot perform signup. (%d)", res);
            error_exit(error);
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

    bool quit = false;
    while (!quit) {
        printf("Please select a command (1-3): ");

        int res;
        char input[3], error[80];
        fgets(input, 3, stdin);

        switch (input[0]) {
            case '1':
                memset(&secret[0], 0, sizeof(secret));

                printf("Please enter the new secret: ");
                fgets(secret, SECRET_LENGTH, stdin);
                secret[strcspn(secret, "\n")] = 0; // remove trailing \n

                if ((res = communicate(request_write, response_write)) < 0) {
                    snprintf(error, sizeof(error), "Cannot write secret. (%d)", res);
                    error_exit(error);
                }

                printf("Secret was successfully updated!\n");
                break;
            case '2':
                memset(&secret[0], 0, sizeof(secret));

                if ((res = communicate(request_read, response_read)) < 0) {
                    snprintf(error, sizeof(error), "Cannot read secret. (%d)", res);
                    error_exit(error);
                }

                printf("Your secret was received successfully:\n%s\n", secret);

                break;
            case '3':
                if ((res = communicate(request_logout, response_logout)) < 0) {
                    snprintf(error, sizeof(error), "Cannot communicate logout. (%d)", res);
                    error_exit(error);
                }
                quit = true;
                printf("Bye.\n");
                break;
            default:
                break;
        }
    }
}

int communicate(
        void (*handle_request)(auth_memory_t *shared, char *username, char *password, char *session, char *secret),
        int (*handle_response)(auth_memory_t *shared, char *username, char *password, char *session, char *secret)
) {
    if (!shared->server_available) {
        error_exit("Server is not available");
    }

    sem_wait(semaphores->mutex); // notify server, that the client wants to interact

    // prepare request
    handle_request(shared, username, password, session, secret);

    sem_post(semaphores->server); // request done

    sem_wait(semaphores->client); // wait for response

    // handle response
    return handle_response(shared, username, password, session, secret);
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
        error_exit("Either option l or r must occur, but not both of them");
    }

    mode = (opt_r == 1) ? MODE_REGISTER : MODE_LOGIN;

    // Check whether there are the correct amount of positional arguments (un, pw) or an invalid option were supplied.
    if ((argc - optind) != 2 || invopt) usage();

    strncpy(username, argv[optind], USERNAME_LENGTH);
    strncpy(password, argv[optind + 1], PASSWORD_LENGTH);
}


static void usage(void) {
    fprintf(stderr, "SYNOPSIS\n\t%s { -r | -l } username password\n", pgm_name);
    exit(EXIT_FAILURE);
}

static void clean_up(void) {
    memory_close(shared);
    semaphores_close(semaphores);
}