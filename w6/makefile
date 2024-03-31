# File name
FN = threads

# Compiler
CC = gcc

# Compiler flags
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -g
CFLAGS += -Werror

# Source files
SOURCES = $(FN).c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Executable name
EXECUTABLE = $(FN)

# Default target
all: $(EXECUTABLE)

# Linking the object files to create the executable
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXECUTABLE)

# Compiling C source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule to remove generated files
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
