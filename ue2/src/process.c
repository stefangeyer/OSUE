#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>
#include "process.h"

#define BUFFER_SIZE (512)

static void tokenize(char *in, const char *delim, char out[][LINE_LENGTH], long int len);

static void print_merge(char left[][LINE_LENGTH], long int llen, char right[][LINE_LENGTH], long int rlen);

static char *read_pipe(int pipefd[]);

static void write_pipe(int pipefd[], char *out, size_t len);

static void fork_recursive(int *pid, int *inpipefd, int *outpipefd);

static void wait_child(int pid);

void forksort(long int amount, char lines[][LINE_LENGTH]) {
    if (amount == 1) {
        printf("%s\n", lines[0]);
        return;
    }

    long int mid = amount / 2 + (amount % 2), lamount = mid, ramount = amount - mid;
    int linpipefd[2], loutpipefd[2], rinpipefd[2], routpipefd[2];
    pid_t lpid, rpid;
    char number[32], *delim = "\n";
    char *lresult = NULL, *rresult = NULL;
    char llines[lamount][LINE_LENGTH], rlines[ramount][LINE_LENGTH];

    // Setup pipes
    if (pipe(linpipefd) == -1 || pipe(loutpipefd) == -1 ||
        pipe(rinpipefd) == -1 || pipe(routpipefd) == -1)
        error_exit("Cannot create pipe");

    // Fork left process
    fork_recursive(&lpid, linpipefd, loutpipefd);

    // Fork right process
    fork_recursive(&rpid, rinpipefd, routpipefd);

    // Close unused pipe ends
    close(linpipefd[0]);
    close(loutpipefd[1]);
    close(rinpipefd[0]);
    close(routpipefd[1]);

    // Write line amount to left pipe
    int b = snprintf(number, 32, "%ld\n", mid);
    write_pipe(linpipefd, number, (size_t) b);

    // Write line amount to right pipe
    b = snprintf(number, 32, "%ld\n", amount - mid);
    write_pipe(rinpipefd, number, (size_t) b);

    // Write the first half of lines to the left pipe and the second to the right pipe
    for (long int i = 0; i < lamount; i++) {
        char *line = lines[i];
        write_pipe(linpipefd, line, strlen(line));
    }

    for (long int i = 0; i < ramount; i++) {
        char *line = lines[mid + i];
        write_pipe(rinpipefd, line, strlen(line));
    }

    // Wait for children
    wait_child(lpid);
    wait_child(rpid);

    // Read result of left child into a single string
    lresult = read_pipe(loutpipefd);
    rresult = read_pipe(routpipefd);

    // Tokenize both results
    tokenize(lresult, delim, llines, lamount);
    tokenize(rresult, delim, rlines, ramount);

    // Merge both results together
    print_merge(llines, lamount, rlines, ramount);

    // Free resources
    free(lresult);
    free(rresult);
}

static void tokenize(char *in, const char *delim, char out[][LINE_LENGTH], long int len) {
    char *token = strtok(in, delim);

    for (int i = 0; token != NULL; i++) {
        if (i == len) break; // out is full
        strncpy(out[i], token, LINE_LENGTH);
        token = strtok(NULL, delim);
    }
}

static void print_merge(char left[][LINE_LENGTH], long int llen, char right[][LINE_LENGTH], long int rlen) {
    long int i = 0, j = 0;

    while (i < llen && j < rlen) {
        char *lline = left[i], *rline = right[j];
        int cmp = strcmp(lline, rline);
        if (cmp <= 0) {
            printf("%s\n", lline);
            i++;
        } else {
            printf("%s\n", rline);
            j++;
        }
    }

    // Copy remaining left elements if there are any
    for (; i < llen; i++) {
        printf("%s\n", left[i]);
    }

    // Copy remaining right elements if there are any
    for (; j < rlen; j++) {
        printf("%s\n", right[j]);
    }
}

static char *read_pipe(int pipefd[]) {
    char buffer[BUFFER_SIZE], *result = NULL;
    int counter = 1;

    // Clear buffer
    memset(&buffer, 0, BUFFER_SIZE);

    // Read result of left child into a single string
    while (read(pipefd[0], &buffer, BUFFER_SIZE) > 0) {
        bool first = result == NULL;
        result = realloc(result, sizeof(char) * BUFFER_SIZE * counter++);
        if (first) strcpy(result, buffer);
        else strcat(result, buffer);
    }

    return result;
}

static void write_pipe(int pipefd[], char *out, size_t len) {
    ssize_t written = 0;
    if ((written = write(pipefd[1], out, len)) == -1) {
        error_exit("Cannot write line to child");
    }
}

static void fork_recursive(int *pid, int *inpipefd, int *outpipefd) {
    *pid = fork();
    if (*pid == -1) error_exit("Cannot fork child");

    if (*pid == 0) {
        // Child process
        close(inpipefd[1]); // Only read from input pipe
        dup2(inpipefd[0], STDIN_FILENO); // Redirect read end to stdin

        close(outpipefd[0]); // Only write to output pipe
        dup2(outpipefd[1], STDOUT_FILENO); // Redirect stdout to write end

        // Recursive call to own process
        execlp(pgm_name, pgm_name, NULL);

        // We only get to this point of exec fails
        error_exit("Child's exec failed");
    }
}

static void wait_child(int pid) {
    int status;

    if (waitpid(pid, &status, 0) == -1) {
        error_exit("Cannot wait for child");
    }

    // Check if child exited successful
    if (WEXITSTATUS(status) != EXIT_SUCCESS) error_exit("Child exited with failure code");
}