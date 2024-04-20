#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h> // Include for error number descriptions

void cd_command(char *path) {
    // Check if the path is NULL or an empty string, in which case default to the HOME directory
    if (path == NULL || strcmp(path, "") == 0) {
        path = getenv("HOME");
        if (path == NULL) {
            fprintf(stderr, "cd: HOME directory not found\n");
            return;
        }
    }

    // Debug output: Print the path we are attempting to change to
    printf("Attempting to change directory to: '%s'\n", path);

    // Attempt to change the directory to 'path'
    if (chdir(path) != 0) {
        // If changing the directory fails, print an error message with the specific reason
        fprintf(stderr, "Error changing directory to %s: %s\n", path, strerror(errno));
    } else {
        // If successful, fetch and print the new current working directory to confirm the change
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Changed directory to: %s\n", cwd);
        } else {
            perror("getcwd() error"); // Print error if getting the current working directory fails
        }
    }
}
