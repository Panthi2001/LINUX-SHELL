#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "pipelining.h"

void execute_pipeline(char *command_line) {
    int pipefd[2];
    char *command[2];
    pid_t pid1, pid2;

    command[0] = strtok(command_line, "|");
    command[1] = strtok(NULL, "");

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork the first child process
    pid1 = fork();
    if (pid1 == 0) {
        close(pipefd[0]); // Close unused read end
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        execlp(command[0], command[0], (char *)NULL);
        perror("exec");
        exit(EXIT_FAILURE);
    }

    // Fork the second child process
    pid2 = fork();
    if (pid2 == 0) {
        close(pipefd[1]); // Close unused write end
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        execlp(command[1], command[1], (char *)NULL);
        perror("exec");
        exit(EXIT_FAILURE);
    }

    // Parent closes both ends and waits for children
    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}
