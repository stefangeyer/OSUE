#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>
#include "process.h"

#define BUFFER_SIZE (512)

void forksort(long int amount, char lines[][LINE_LENGTH], char *pgm_name) {
    if (amount == 1) {
        printf("%s", lines[0]);
        return;
    }

    long int mid = amount / 2 + (5 % 2);
    int linpipefd[2], loutpipefd[2], rinpipefd[2], routpipefd[2], status, rcounter = 1;
    pid_t lpid, rpid;
    char number[32], buffer[BUFFER_SIZE], *lresult = NULL, *rresult = NULL;
    char *ltoken, *rtoken, *lsave, *rsave;

    // Setup pipes
    if (pipe(linpipefd) == -1 || pipe(loutpipefd) == -1 ||
        pipe(rinpipefd) == -1 || pipe(routpipefd) == -1)
        error_exit("Cannot create pipe");

    // Fork left process
    lpid = fork();
    if (lpid == -1) error_exit("Cannot fork left child");

    if (lpid == 0) {
        // Left child

        close(linpipefd[1]); // Only read from input pipe
        dup2(linpipefd[0], STDIN_FILENO); // Redirect read end to stdin

        close(loutpipefd[0]); // Only write to output pipe
        dup2(loutpipefd[1], STDOUT_FILENO); // Redirect stdout to write end

        // Recursive call to own process
        execlp(pgm_name, pgm_name, NULL);

        // We only get to this point of exec fails
        error_exit("Left child's exec failed");
    }

    // Parent

    // Fork right process
    rpid = fork();
    if (rpid == -1) error_exit("Cannot fork right child");

    if (rpid == 0) {
        // Right child

        close(rinpipefd[1]); // Only read from input pipe
        dup2(rinpipefd[0], STDIN_FILENO); // Redirect read end to stdin

        close(routpipefd[0]); // Only write to output pipe
        dup2(routpipefd[1], STDOUT_FILENO); // Redirect stdout to write end

        // Recursive call to own process
        execlp(pgm_name, pgm_name, NULL);

        // We only get to this point of exec fails
        error_exit("Right child's exit failed");
    }

    // Parent

    // Close unused pipe ends
    close(linpipefd[0]);
    close(loutpipefd[1]);
    close(rinpipefd[0]);
    close(routpipefd[1]);

    // Write line amount to left pipe
    snprintf(number, 32, "%ld", mid);
    write(linpipefd[1], number, 32);

    // Write line amount to right pipe
    snprintf(number, 32, "%ld", amount - mid);
    write(rinpipefd[1], number, 32);

    // Write the first half of lines to the left pipe and the second to the right pipe
    for (long int i = 0; i < amount; i++) {
        int fd;
        if (i < mid) fd = linpipefd[1];
        else fd = rinpipefd[1];

        write(fd, lines[i], LINE_LENGTH);
        write(fd, "\n", 2);
    }

    // Wait for children

    /* while ((pid = wait(&status)) != lpid) {
        if (pid != -1) continue;
        // other child
        if (errno == EINTR) continue;
        // interrupted
        error_exit("Cannot wait for left child!");
    } */

    if (waitpid(lpid, &status, 0) == -1) {
        error_exit("Cannot wait for left child");
    }

    // Check if child left child exited successful
    if (WEXITSTATUS(status) != EXIT_SUCCESS) error_exit("Left child exited with failure code");

    /* while ((pid = wait(&status)) != rpid) {
        if (pid != -1) continue;
        // other child
        if (errno == EINTR) continue;
        // interrupted
        error_exit("Cannot wait for right child!");
    } */

    if (waitpid(rpid, &status, 0) == -1) {
        error_exit("Cannot wait for right child");
    }

    // Check if child left child exited successful
    if (WEXITSTATUS(status) != EXIT_SUCCESS) error_exit("Right child exited with failure code");

    // Read result of left child
    while (read(loutpipefd[0], &buffer, BUFFER_SIZE) > 0) {
        bool first = lresult == NULL;
        lresult = realloc(lresult, (size_t) (BUFFER_SIZE * rcounter++));
        if (first) lresult[0] = 0;
        strcat(lresult, buffer);
    }

    rcounter = 1;

    // Read result of right child
    while (read(routpipefd[0], &buffer, BUFFER_SIZE) > 0) {
        bool first = rresult == NULL;
        rresult = realloc(rresult, (size_t) (BUFFER_SIZE * rcounter++));
        if (first) rresult[0] = 0;
        strcat(rresult, buffer);
    }

    // Tokenize both results and perform merge

    ltoken = strtok_r(lresult, "\n", &lsave);
    rtoken = strtok_r(rresult, "\n", &rsave);

    while (ltoken != NULL && rtoken != NULL) {
        if (strcmp(ltoken, rtoken) < 0) {
            printf(ltoken);
            ltoken = strtok_r(NULL, "\n", &lsave);
        } else {
            printf(rtoken);
            rtoken = strtok_r(NULL, "\n", &rsave);
        }
    }

    // Free resources
    free(lresult);
    free(rresult);
}