#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "re_execution_engine.h"

void re_execute_command(const char *cmd, char *const argv[], const char *input_file, const char *output_file) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return;
    }

    if (pid == 0) {  // Child process
        // Input redirection
        if (input_file) {
            int in_fd = open(input_file, O_RDONLY);
            if (in_fd == -1) {
                perror("Failed to open input file");
                exit(EXIT_FAILURE);
            }
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }

        // Output redirection
        if (output_file) {
            int out_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (out_fd == -1) {
                perror("Failed to open output file");
                exit(EXIT_FAILURE);
            }
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }

        execvp(cmd, argv);
        //perror("execvp failed");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)==1) {
            printf("success redirection\n");
        }
    }
}
