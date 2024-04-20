#ifndef EXECUTION_ENGINE_H
#define EXECUTION_ENGINE_H

// Executes a command with arguments, handling input/output redirection if specified
void re_execute_command(const char *cmd, char *const argv[], const char *input_file, const char *output_file);

#endif /* EXECUTION_ENGINE_H */
