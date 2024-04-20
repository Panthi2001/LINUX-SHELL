#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to display the current pathnames
void show_path() {
    char *path = getenv("PATH");
    if (path != NULL) {
        printf("Pathnames: %s\n", path);
    } else {
        printf("Pathnames: (empty)\n");
    }
}

// Function to append a pathname to the path variable
void add_path(const char *pathname) {
    char *current_path = getenv("PATH");
    char *new_path;

    if (current_path == NULL) {
        // If PATH variable is empty, set the new path directly
        new_path = strdup(pathname);
    } else {
        // Otherwise, concatenate the new pathname to the current PATH variable
        size_t len = strlen(current_path) + strlen(pathname) + 2;
        new_path = malloc(len);
        if (new_path == NULL) {
            perror("malloc");
            return;
        }
        snprintf(new_path, len, "%s:%s", current_path, pathname);
    }

    // Set the modified PATH variable
    setenv("PATH", new_path, 1);
    free(new_path);
}

// Function to remove a pathname from the path variable
void remove_path(const char *pathname) {
    char *current_path = getenv("PATH");
    if (current_path == NULL) {
        printf("Pathnames: (empty)\n");
        return;
    }

    // Find the position of the specified pathname in the current path list
    char *path_position = strstr(current_path, pathname);
    if (path_position != NULL) {
        // Remove the specified pathname from the current path list
        size_t path_len = strlen(pathname);
        if (path_position == current_path) {
            // If the specified pathname is at the beginning of the path list
            if (path_position[path_len] == ':') {
                // Skip the colon delimiter
                path_position++;
                path_len++;
            }
        } else if (path_position[path_len] == ':' && path_position[-1] == ':') {
            // If the specified pathname is in the middle of the path list
            path_position--;
            path_len++;
        }

        // Move the remaining path list over the removed pathname
        memmove(path_position, path_position + path_len, strlen(path_position + path_len) + 1);

        // Set the modified PATH variable
        setenv("PATH", current_path, 1);
    } else {
        printf("Pathnames: %s\n", current_path);
    }
}
