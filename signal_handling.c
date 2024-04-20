#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "signal_handling.h"

// Global variable to hold the foreground process group ID
extern pid_t foreground_process_group;

// Signal handler function for SIGINT (Ctrl-C)
void sigint_handler(int sig) {
    (void)sig;
    // Forward SIGINT to the foreground process group
    kill(-foreground_process_group, SIGINT);
}

// Signal handler function for SIGTSTP (Ctrl-Z)
void sigtstp_handler(int sig) {
	printf("Ctrl+Z signal received\n"); // Diagnostic print
    (void)sig;

    // Forward SIGTSTP to the foreground process group
    kill(-foreground_process_group, SIGTSTP);
}

// Function to set up signal handling
void setup_signal_handling() {
    // Set signal handlers for SIGINT and SIGTSTP
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);
}

// Function to spawn subprocesses
pid_t spawn_process(const char *command) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        // Set its process group ID to the saved foreground process group ID
        setpgid(0, foreground_process_group);
        
        // Execute the command
        execlp(command, command, NULL);
        
        // If execlp fails, print an error and exit
        perror("execlp");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent process
        // Do nothing for now
    } else {
        // Fork failed
        perror("fork");
    }
    return pid;
}
