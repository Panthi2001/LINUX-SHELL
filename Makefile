# Compiler
CC = gcc
# Compiler flags
CFLAGS = -Wall -Wextra -g

# Source files
SRCS = cd_command.c exit_command.c main.c pipelining.c signal_handling.c alias.c   myhistory.c path_command.c re_execution_engine.c
# Object files
OBJS = $(SRCS:.c=.o)
# Executable
EXEC = newshell

# Default rule
all: $(EXEC)

# Rule to build the executable
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC)

# Rule to compile C files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(EXEC)
