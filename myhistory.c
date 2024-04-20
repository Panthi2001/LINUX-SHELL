#include <stdio.h>
#include <string.h>
#include "myhistory.h"


#define MAX_HISTORY_SIZE 20

// Function prototype for execute_command
void execute_command(const char *command);

// Structure to represent a command in the history
typedef struct {
    int number;  // Command number
    char command[512]; // Command string
} HistoryEntry;

// Array to store the history of commands
static HistoryEntry history[MAX_HISTORY_SIZE];
static int history_size = 0;

void add_to_history(const char *command) {
    // Shift existing history entries to make room for the new command
    if (history_size == MAX_HISTORY_SIZE) {
        for (int i = 0; i < MAX_HISTORY_SIZE - 1; i++) {
            history[i] = history[i + 1];
        }
        history_size--;
    }

    // Add the new command to the history
    strcpy(history[history_size].command, command);
    history[history_size].number = history_size + 1;
    history_size++;
}

void display_history() {
    printf("Shell command history:\n");
    for (int i = 0; i < history_size; i++) {
        printf("%d: %s\n", history[i].number, history[i].command);
    }
}

void clear_history() {
    history_size = 0;
    printf("Shell command history cleared\n");
}

void execute_from_history(int number) {
    if (number > 0 && number <= history_size) {
        execute_command(history[number - 1].command);
    } else {
        printf("Invalid history number\n");
    }
}
