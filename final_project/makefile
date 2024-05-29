# File name
FN = try2

# Compiler
CC = gcc

# Compiler flags
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -g
CFLAGS += -Werror
# CFLAGS += -lpthread

LDFLAGS += -lmad 
LDFLAGS += -lmp3lame

# Include directories
INCLUDES += -I/usr/include/lame

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
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

# Compiling C source files into object files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean rule to remove generated files
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

# Run target
run:
	./$(EXECUTABLE) input.mp3 output.mp3 1.8
	cp output.mp3 /mnt/d/output.mp3
