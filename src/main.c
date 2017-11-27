#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#define BUF_SIZE 1024

static char *pgm_name; /**< program name */
static char *directory;
static char *prefix = "";

int main(int argc, char *argv[]) {
    parse_arguments(argc, argv); // this also sets pgm_name

    int status, pipefd[2];
    pid_t pid, child_pid;

    if (pipe(pipefd) == -1) error_exit("Cannot create pipe!");

    child_pid = fork();
    if (child_pid == -1) error_exit("Cannot fork!");

    if (child_pid == 0) {
        // Child
        close(pipefd[0]); // child does not read
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
        char *cmd[] = {"ls", "-1a", directory, NULL}; // Must must contain argv[0] and end with a NULL pointer
        execvp("ls", cmd);
        error_exit("Cannot exec!");
    } else {
        // Parent
        close(pipefd[1]); // parent does not write to pipe
        char buf[BUF_SIZE], *token, *delim = "\n";
        while (read(pipefd[0], &buf, BUF_SIZE) > 0) {
            token = strtok(buf, delim);
            while (token != NULL) {

                token = strtok(NULL, delim);
            }
        }
        close(pipefd[0]);

        // Wait for the child process to finish
        while ((pid = wait(&status)) != child_pid) {
            if (pid != -1) continue;
            // other child
            if (errno == EINTR) continue;
            // interrupted
            error_exit("Cannot wait!");
        }
        if (WEXITSTATUS(status) == EXIT_SUCCESS) {

        }
    }

    return EXIT_SUCCESS;
}

void usage(void) {
    fprintf(stderr, "SYNOPSIS:\n\t%s [-i ignorepraefix] <directory>", pgm_name);
    exit(EXIT_FAILURE);
}

void error_exit(char *error) {
    fprintf(stderr, "%s\n", error);
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