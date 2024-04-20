#ifndef MYHISTORY_H
#define MYHISTORY_H

void add_to_history(const char *command);
void display_history();
void clear_history();
void execute_from_history(int number);

#endif /* MYHISTORY_H */
