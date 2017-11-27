#include <sched.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include <stdlib.h>
#include "child.h"
#include "main.h"

void child_process(void (*parent)(int[]), void (*child)(int[])) {
    int status, pipefd[2];
    pid_t pid, child_pid;

    if (pipe(pipefd) == -1) error_exit("Cannot create pipe!");

    child_pid = fork();
    if (child_pid == -1) error_exit("Cannot fork!");

    if (child_pid == 0) {
        // Child
        child(pipefd);
    } else {
        // Parent
        parent(pipefd);

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
}