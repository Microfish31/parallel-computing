# File name
FN = obmt

# Compiler
NVCC = nvcc

# Compiler flags
NVCCFLAGS += -arch=compute_50  # Specify your GPU architecture
NVCCFLAGS += -Xcompiler -Wall
NVCCFLAGS += -Xcompiler -Wextra
NVCCFLAGS += -Xcompiler -g
NVCCFLAGS += -Xcompiler -Werror
# NVCCFLAGS += -Xcompiler -Wl
# NVCCFLAGS += -Xcompiler -lrt

# Source files
SOURCES = $(FN).cu

# Object files
OBJECTS = $(SOURCES:.cu=.o)

# Executable name
EXECUTABLE = $(FN)

# Default target
all: $(EXECUTABLE)

# Linking the object files to create the executable
$(EXECUTABLE): $(OBJECTS)
	$(NVCC) $(NVCCFLAGS) $(OBJECTS) -o $(EXECUTABLE)

# Compiling CUDA source files into object files
%.o: %.cu
	$(NVCC) $(NVCCFLAGS) -c $< -o $@

# Clean rule to remove generated files
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
