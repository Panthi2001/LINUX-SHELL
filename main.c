#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <signal.h>

#include "cd_command.h"
#include "pipelining.h"
#include "exit_command.h"
#include "signal_handling.h"
#include "alias.h" // Include the alias header file
#include "myhistory.h"
#include "path_command.h"
#include "re_execution_engine.h" // Include the header file for path command

// Global variable to hold the foreground process
pid_t foreground_process_group = 0;

// Flag to indicate whether the mode has been executed
int mode_executed = 0;

// Function to print shell prompt
void prompt() {
    printf("myshell> ");
    fflush(stdout);
}

// Function to check if input/output redirection is required
int input_output_redirection_required(const char *command) {
    // Your logic to determine if input/output redirection is required
    // For example, you can check if the command contains "<" or ">"
    if (strstr(command, "<") || strstr(command, ">")) {
        return 1; // Input/output redirection is required
    } else {
        return 0; // Input/output redirection is not required
    }
}

// Function to trim leading and trailing whitespace
char *trim_whitespace(char *str) {
    while (isspace((unsigned char)*str)) str++; // Trim leading space
    if (*str == 0) return str; // All spaces?
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--; // Trim trailing space
    end[1] = '\0'; // Write new null terminator
    return str;
}

// Function to execute a command
void execute_command(const char *command) {
    // Check if the command is an alias
    for (int j = 0; j < num_aliases; j++) {
        if (strcmp(command, aliases[j].alias_name) == 0) {
            printf("Alias '%s' detected, executing '%s'\n", command, aliases[j].command);
            // Execute the alias command
            system(aliases[j].command);
            return;
        }
    }

    // If the command is not an alias, proceed with regular command execution
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        // Restore default signal handling behavior for subprocesses
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);

        // Set the child process group ID to its own PID
        setpgid(0, 0);

        // Execute the command using system()
        system(command);

        // Exit the child process
        exit(EXIT_SUCCESS);
    } else if (pid > 0) {
        foreground_process_group = pid;
        // Parent process
        int status;
        // Wait for the child process to finish
        waitpid(pid, &status, 0);
    } else {
        // Fork failed
        perror("fork");
    }
}

// Function to run the shell in interactive mode
void interactive_mode() {
    char input[512];
    char *token;

    // Initialize signal handling
    setup_signal_handling();

    // Display prompt and read input until the user exits
    while (1) {
        prompt();
        if (!fgets(input, 512, stdin)) break; // Break on EOF

        // Remove trailing newline character
        input[strcspn(input, "\n")] = 0;

        // Handle empty input
        if (strcmp(input, "") == 0) continue;

        // Add command to history
        add_to_history(input);

        // Parse and execute commands separated by semicolons
        token = strtok(input, ";");
        while (token != NULL) {
            // Trim leading and trailing whitespace
            char *trimmed_token = trim_whitespace(token);

            if (strncmp(trimmed_token, "exit", 4) == 0) {
                exit_shell(); // Call the function that executes the exit command
            } else if (strncmp(trimmed_token, "alias", 5) == 0) {
                // Handle alias command
                char *alias_cmd = trimmed_token + 5; // Skip 'alias'
                while (*alias_cmd == ' ') alias_cmd++; // Skip spaces after 'alias'
                if (*alias_cmd == '\0') {
                    // Display existing aliases
                    display_aliases();
                } else if (strncmp(alias_cmd, "-r", 2) == 0) {
                    // Remove alias
                    char *alias_name = alias_cmd + 2; // Skip '-r'
                    while (*alias_name == ' ') alias_name++; // Skip spaces after '-r'
                    if (*alias_name != '\0') {
                        remove_alias(alias_name);
                    } else {
                        printf("Usage: alias -r alias_name\n");
                    }
                } else if (strncmp(alias_cmd, "-c", 2) == 0) {
                    // Clear all aliases
                    num_aliases = 0;
                    printf("All aliases cleared\n");
                } else {
                    // Set alias
                    char *alias_name = alias_cmd;
                    char *equal_sign = strchr(alias_name, '=');
                    if (equal_sign != NULL) {
                        *equal_sign = '\0'; // Terminate alias name
                        char *command = equal_sign + 1;
                        while (*command == ' ') command++; // Skip spaces after '='
                        set_alias(alias_name, command);
                    } else {
                        printf("Invalid alias syntax\n");
                    }
                }
            } else if (strncmp(trimmed_token, "myhistory", 9) == 0) {
                // Handle myhistory command
                char *args = trimmed_token + 9; // Skip 'myhistory'
                while (*args == ' ') args++; // Skip spaces after 'myhistory'
                if (*args == '\0') {
                    // Display history
                    display_history();
                } else if (strncmp(args, "-c", 2) == 0) {
                    // Clear history
                    clear_history();
                } else if (strncmp(args, "-e", 2) == 0) {
                    // Execute command from history
                    int number;
                    if (sscanf(args + 2, "%d", &number) == 1) {
                        execute_from_history(number);
                    } else {
                        printf("Invalid myhistory syntax\n");
                    }
                } else {
                    printf("Invalid myhistory command\n");
                }
            } else if (strstr(trimmed_token, "|")) {
                execute_pipeline(trimmed_token);
            } else if (strncmp(trimmed_token, "path", 4) == 0) {
                // Handle path commands
                char *arg = trimmed_token + 4;
                while (*arg == ' ') arg++; // Skip spaces after 'path'
                if (*arg == '\0') {
                    // Display pathnames if no arguments provided
                    show_path();
                } else if (strncmp(arg, "+", 1) == 0) {
                    // Append path if the argument starts with '+'
                    arg += 1; // Move to the actual pathname
                    while (*arg == ' ') arg++; // Skip spaces after '+'
                    add_path(arg); // Add pathname
                } else if (strncmp(arg, "-", 1) == 0) {
                    // Remove path if the argument starts with '-'
                    arg += 1; // Move to the actual pathname
                    while (*arg == ' ') arg++; // Skip spaces after '-'
                    remove_path(arg); // Remove pathname
                } else {
                    printf("Invalid path command\n");
                }
            } else if (strncmp(trimmed_token, "cd", 2) == 0) {
                // More robust 'cd' command handling
                char *path = trimmed_token + 2;
                while (*path == ' ') path++; // Skip spaces to find the start of the path
                if (*path == '\0') path = NULL; // If no path after 'cd ', set path to NULL
                cd_command(path);
            } else {
                if (input_output_redirection_required(trimmed_token)) {
                    re_execute_command(trimmed_token, NULL, NULL, NULL); // For commands with input/output redirection
                } else {
                    execute_command(trimmed_token); // For commands without input/output redirection
                }
            }
            token = strtok(NULL, ";");
        }
    }
}

// Function to run the shell in batch mode
void batch_mode(const char *batch_file) {
    FILE *file = fopen(batch_file, "r");
    if (file == NULL) {
        perror("Failed to open batch file");
        exit(EXIT_FAILURE);
    }

    char line[512];
    while (fgets(line, sizeof(line), file)) {
        // Remove trailing newline character
        line[strcspn(line, "\n")] = 0;

        // Echo the command back to the user
        printf("%s\n", line);

        // Execute the command
        char *trimmed_line = trim_whitespace(line);
        if (strlen(trimmed_line) > 0) {
            if (strcmp(trimmed_line, "exit") == 0) {
                printf("Exiting batch mode...\n");
                fclose(file);
                return; // Exit the function and batch mode
            }
            if (input_output_redirection_required(trimmed_line)) {
                re_execute_command(trimmed_line, NULL, NULL, NULL); // For commands with input/output redirection
            } else {
                execute_command(trimmed_line); // For commands without input/output redirection
            }
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (mode_executed == 0) {
        if (argc == 1) {
            // No command-line arguments, run in interactive mode
            interactive_mode();
            mode_executed = 1; // Set the flag to indicate that the mode has been executed
        } else if (argc == 2) {
            // One command-line argument, run in batch mode
            batch_mode(argv[1]);
            mode_executed = 1; // Set the flag to indicate that the mode has been executed
        } else {
            printf("Usage: %s [batch_file]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
