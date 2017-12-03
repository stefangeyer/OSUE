#include <sched.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include "process.h"
#include "main.h"

char_array_t *list_directory(char *directory) {
    int status, pipefd[2], i = 0;
    pid_t pid, child_pid;
    char **array = NULL;

    if (pipe(pipefd) == -1) error_exit("Cannot create pipe!");

    child_pid = fork();
    if (child_pid == -1) error_exit("Cannot fork!");

    if (child_pid == 0) {
        // Child
        close(pipefd[0]); // child does not read
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
        char *cmd[] = {"ls", "-1a", directory, NULL}; // Must must contain argv[0] and end with a NULL pointer
        execvp("ls", cmd);
        // Only gets here when exec fails
        error_exit("Cannot exec!");
    } else {
        // Parent
        // Wait for the child process to finish
        while ((pid = wait(&status)) != child_pid) {
            if (pid != -1) continue;
            // other child
            if (errno == EINTR) continue;
            // interrupted
            error_exit("Cannot wait!");
        }
        // Check if child was successful
        if (WEXITSTATUS(status) == EXIT_SUCCESS) {
            close(pipefd[1]); // parent does not write to pipe
            char buf[BUF_SIZE], *token, *delim = "\n";
            while (read(pipefd[0], &buf, BUF_SIZE) > 0) {
                token = strtok(buf, delim);
                for (; token != NULL; i++) {
                    array = realloc(array, sizeof(char *) * (i + 1));
                    array[i] = malloc(strlen(token) + 1);
                    strcpy(array[i], token);
                    token = strtok(NULL, delim);
                }
            }
            close(pipefd[0]);

        } else {
            error_exit("Cannot list directory!");
        }
    }

    if (array == NULL) return NULL;

    char_array_t *out = malloc(sizeof(char_array_t));
    out->array = array;
    out->size = i;

    return out;
}

char *file_info(char *directory, char *file) {
    int status, pipefd[2];
    pid_t pid, child_pid;
    char *out = NULL, *cmd = "file";

    if (pipe(pipefd) == -1) error_exit("Cannot create pipe!");

    printf("%s%s%s", directory, "/", file);

    child_pid = fork();
    if (child_pid == -1) error_exit("Cannot fork!");

    if (child_pid == 0) {
        // Child
        close(pipefd[0]); // child does not read
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe

        char f[1024] = "";
        snprintf(f, sizeof(f), "%s%s%s", directory, "/", file);

        char *args[] = {cmd, f, NULL}; // Must must contain argv[0] and end with a NULL pointer
        execvp(cmd, args);
        // Only gets here when exec fails
        error_exit("Cannot exec!");
    } else {
        // Parent
        // Wait for the child process to finish
        while ((pid = wait(&status)) != child_pid) {
            if (pid != -1) continue;
            // other child
            if (errno == EINTR) continue;
            // interrupted
            error_exit("Cannot wait!");
        }
        // Check if child was successful
        if (WEXITSTATUS(status) == EXIT_SUCCESS) {
            close(pipefd[1]); // parent does not write to pipe

            char buf[BUF_SIZE];

            // Single line output
            if (read(pipefd[0], &buf, BUF_SIZE) >= 0) {
                out = malloc(strlen(buf) + 1);
                strcpy(out, buf);
            } else error_exit("Cannot read from pipe!");

            close(pipefd[0]);

        } else {
            error_exit("Cannot get file info!");
        }
    }

    return out;
}

char *md5sum(char *directory, char *file) {
    int status, pipefd[2];
    pid_t pid, child_pid;
    char *out = NULL, *cmd = "md5sum";

    if (pipe(pipefd) == -1) error_exit("Cannot create pipe!");

    child_pid = fork();
    if (child_pid == -1) error_exit("Cannot fork!");

    if (child_pid == 0) {
        // Child
        close(pipefd[0]); // child does not read
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe

        char f[1024] = "";
        snprintf(f, sizeof(f), "%s%s%s", directory, "/", file);

        char *args[] = {cmd, f, NULL}; // Must must contain argv[0] and end with a NULL pointer
        execvp(cmd, args);
        // Only gets here when exec fails
        error_exit("Cannot exec!");
    } else {
        // Parent
        // Wait for the child process to finish
        while ((pid = wait(&status)) != child_pid) {
            if (pid != -1) continue;
            // other child
            if (errno == EINTR) continue;
            // interrupted
            error_exit("Cannot wait!");
        }
        // Check if child was successful
        if (WEXITSTATUS(status) == EXIT_SUCCESS) {
            close(pipefd[1]); // parent does not write to pipe

            char buf[BUF_SIZE];

            // Single line output
            if (read(pipefd[0], &buf, BUF_SIZE) >= 0) {
                out = malloc(strlen(buf) + 1);
                strcpy(out, buf);
            } else error_exit("Cannot read from pipe!");

            close(pipefd[0]);

        } else {
            // Handling a directory
            return NULL;
        }
    }

    return out;
}

void free_array(char_array_t *array) {
    if (array == NULL) return;

    // Free all allocated resources
    for (int i = 0; i < array->size; i++) free(array->array[i]);
    free(array->array);
    free(array);
}