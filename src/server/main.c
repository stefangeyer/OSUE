#include <stdbool.h>
#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "main.h"

static char *pgm_name;
static char *db_file_name = NULL;
static node_t *head = NULL;
static auth_memory_t *shared;
static auth_semaphores_t *semaphores;
static bool quit = false;

/**
 * Program entry point.
 * @brief This function takes care about parameters, creates shared memory and semaphores and then waits for incoming requests
 * @details Uses semaphores for the critical section
 * global variables: db_file_name, head, shared, semaphores, quit
 * @param argc The argument counter.
 * @param argv The argument vector.
 * @return Returns EXIT_SUCCESS or exits via a called function
 */
int main(int argc, char *argv[]) {
    // This also handles setting pgm_name
    parse_arguments(argc, argv);
    
    if (atexit(clean_up) != 0) {
        error_exit("Cannot define cleanup function");
    }

    if (db_file_name != NULL) head = read_node(db_file_name);

    shared = memory_create();
    semaphores = semaphores_create();

    printf("Server started. Waiting for incoming queries...\n");

    while (!quit) {
        shared->server_available = true; // tell clients, that the server is available

        // Make sure only one client interacts at a time
        if (sem_post(semaphores->mutex) == -1) error_exit(ERROR_INTERRUPTED);

        // Wait for the client to send its request
        if (sem_wait(semaphores->server) == -1) {
            // interrupted by system call?
            if (errno == EINTR) continue;
            error_exit(ERROR_INTERRUPTED);
        }

        // read request from shm and write response to shm
        switch (shared->state) {
            case REQUEST_LOGIN:
                attempt_login();
                printf("Processed login");
                break;
            case REQUEST_REGISTER:
                attempt_register();
                printf("Processed signup");
                break;
            case REQUEST_READ:
                attempt_read();
                printf("Processed read");
                break;
            case REQUEST_WRITE:
                attempt_write();
                printf("Processed write");
                break;
            case REQUEST_LOGOUT:
                attempt_logout();
                printf("Processed logout");
                break;
            default:
                fprintf(stderr, "Client sent an invalid request code: %d", shared->state);
                break;
        }

        // notify the client that the response is ready
        if (sem_post(semaphores->client) == -1) error_exit(ERROR_INTERRUPTED);
    }

    return EXIT_SUCCESS;
}

static void attempt_login() {
    node_t *user = search_node_for(head, FIELD_USERNAME, shared->username);

    if (user == NULL || strcmp(user->password, shared->password) != 0) {
        shared->state = RESPONSE_LOGIN_FAILURE;
        return;
    }

    char tmp_session[SESSION_LENGTH];
    for(int i = 0; i < SESSION_LENGTH; i++) {
        sprintf(tmp_session + i, "%x", (int) random() % 16); // Generate random hex string as session id
    }

    strcpy(user->session, tmp_session);
    strcpy(shared->session, tmp_session);
    shared->state = RESPONSE_LOGIN_SUCCESS;
}

static void attempt_register() {
    node_t *user = search_node_for(head, FIELD_USERNAME, shared->username);

    if (user != NULL) {
        shared->state = RESPONSE_REGISTER_FAILURE;
        return;
    }

    node_t *new_node = create_node(shared->username, shared->password, "");
    head = append_node(head, new_node);
    print_node(new_node); // TODO debug

    shared->state = RESPONSE_REGISTER_SUCCESS;
}

static void attempt_read() {
    node_t *user = search_node_for(head, FIELD_USERNAME, shared->username);

    if (user == NULL) {
        shared->state = RESPONSE_READ_FAILURE;
        return;
    } else if (strcmp(shared->session, user->session) != 0) {
        shared->state = RESPONSE_INVALID_SESSION;
        return;
    }

    strcpy(shared->secret, user->secret);
    shared->state = RESPONSE_READ_SUCCESS;
}

static void attempt_write() {
    node_t *user = search_node_for(head, FIELD_USERNAME, shared->username);

    if (user == NULL) {
        shared->state = RESPONSE_WRITE_FAILURE;
        return;
    } else if (strcmp(shared->session, user->session) != 0) {
        shared->state = RESPONSE_INVALID_SESSION;
        return;
    }

    strcpy(user->secret, shared->secret);
    shared->state = RESPONSE_WRITE_SUCCESS;
}

static void attempt_logout() {
    node_t *user = search_node_for(head, FIELD_USERNAME, shared->username);

    if (user == NULL) {
        shared->state = RESPONSE_LOGOUT_FAILURE;
        return;
    } else if (strcmp(shared->session, user->session) != 0) {
        shared->state = RESPONSE_INVALID_SESSION;
        return;
    }

    strcpy(user->secret, shared->secret);
    shared->state = RESPONSE_LOGOUT_SUCCESS;
}

static void parse_arguments(int argc, char *argv[]) {
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
        error_exit("Database option can only be set once.");
    }

    // Check whether there are the correct amount of positional arguments or an invalid option were supplied.
    if ((argc - optind) != 0 || invopt) usage();
}

static void usage(void) {
    fprintf(stderr, "SYNOPSIS\n\t%s [-l database]\n", pgm_name);
    exit(EXIT_FAILURE);
}

static void clean_up() {
    shared->server_available = false; // tell clients to disconnect

    memory_destroy(shared);
    semaphores_destroy(semaphores);

    write_node(head, "auth-server.db.csv");
    destroy_node(head);
}