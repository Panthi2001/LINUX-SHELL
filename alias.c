#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alias.h"

#define MAX_ALIASES 100

// Array to store aliases
struct Alias aliases[MAX_ALIASES];
int num_aliases = 0;

// Function to set an alias
void set_alias(const char *alias_name, const char *command) {
    if (num_aliases < MAX_ALIASES) {
        aliases[num_aliases].alias_name = strdup(alias_name);
        aliases[num_aliases].command = strdup(command);
        num_aliases++;
        printf("Alias '%s' set to '%s'\n", alias_name, command);
    } else {
        printf("Maximum number of aliases reached\n");
    }
}

// Function to remove an alias
void remove_alias(const char *alias_name) {
    for (int i = 0; i < num_aliases; i++) {
        if (strcmp(aliases[i].alias_name, alias_name) == 0) {
            free(aliases[i].alias_name);
            free(aliases[i].command);
            // Move the last alias to the current position
            aliases[i] = aliases[num_aliases - 1];
            num_aliases--;
            printf("Alias '%s' removed\n", alias_name);
            return;
        }
    }
    printf("Alias '%s' not found\n", alias_name);
}

// Function to display all existing aliases
void display_aliases() {
    printf("Existing aliases:\n");
    for (int i = 0; i < num_aliases; i++) {
        printf("%s='%s'\n", aliases[i].alias_name, aliases[i].command);
    }
}
