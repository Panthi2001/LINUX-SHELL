#ifndef ALIAS_H
#define ALIAS_H

struct Alias {
    char *alias_name;
    char *command;
};

extern struct Alias aliases[];
extern int num_aliases;
// Function to set an alias
void set_alias(const char *alias_name, const char *command);

// Function to remove an alias
void remove_alias(const char *alias_name);

// Function to display all existing aliases
void display_aliases();

#endif /* ALIAS_H */
