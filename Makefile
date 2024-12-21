# # Compiler
# CC = gcc                            # Use GNU C Compiler


# # Compiler Flags
# CFLAGS = -Wall -g -std=c99          # Warnings enabled, debug symbols, C99 standard
# # -Wall: All warnings
# # -g: Generate debug information
# # -std=c99: Use C99 standard


# # Linker Flags
# LDFLAGS = -lcrypto -lm              # Link with OpenSSL crypto library and math library
# # -lcrypto: OpenSSL cryptography library
# # -lm: Math library for mathematical functions


# # Executable name
# EXECUTABLE = compressor              # Name of the final executable


# # Source files
# SOURCES = main.c \
#           rle/rle_compress.c \
#           rle/rle_decompress.c \
#           utils/bit_manipulation.c \
#           huffman/huffman_compress.c \
#           huffman/huffman_decompress.c \
#           reports/compression_report.c \
#           archive/archive.c \
#           encryption/encryption.c \
#           benchmark/benchmark.c
# # List of all source files in the project


# # Object files (automatically generated from source files)
# OBJECTS = $(SOURCES:.c=.o)
# # Replaces .c extension with .o for each source file
# # Example: main.c -> main.o


# # Default target
# all: $(EXECUTABLE)
# # This is the default goal when 'make' is run without arguments


# # Link object files into executable
# $(EXECUTABLE): $(OBJECTS)
#     $(CC) $(CFLAGS) -o $(EXECUTABLE) $(OBJECTS) $(LDFLAGS)
# # Compile all object files into the final executable
# # $(CFLAGS) applied during compilation
# # $(LDFLAGS) used for linking external libraries


# # Compile source files into object files
# %.o: %.c
#     $(CC) $(CFLAGS) -c $< -o $@
# # Pattern rule to generate object files from source files
# # $< is the first prerequisite (source file)
# # $@ is the target (object file)
# # -c flag tells gcc to compile but not link


# # Clean up generated files
# clean:
#     rm -f $(OBJECTS) $(EXECUTABLE)
# # Remove all object files and the executable


# # Declare phony targets
# .PHONY: all clean
# # Prevents conflicts if files named 'all' or 'clean' exist
# # Ensures these targets always run their commands

# v2

# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -Wall -g -std=c99

# Linker Flags
LDFLAGS = -lcrypto -lm

# Executable Name
EXECUTABLE = compressor

# Source Files
SOURCES = \
    main.c \
    rle/rle_compress.c \
    rle/rle_decompress.c \
    utils/bit_manipulation.c \
    huffman/huffman_compress.c \
    huffman/huffman_decompress.c \
    reports/compression_report.c \
    archive/archive.c \
    encryption/encryption.c \
    benchmark/benchmark.c

# Object Files (automatically generated from source files)
OBJECTS = $(SOURCES:.c=.o)

# Default Target
all: $(EXECUTABLE)

# Link Object Files into Executable
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(OBJECTS) $(LDFLAGS)

# Compile Source Files into Object Files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean Up Generated Files
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

# Declare Phony Targets
.PHONY: all clean
