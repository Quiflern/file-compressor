# Makefile for the file compressor project

CC = gcc
CFLAGS = -Wall -g
EXECUTABLE = compressor

SOURCES = main.c \
          rle/rle_compress.c \
          rle/rle_decompress.c \
          utils/bit_manipulation.c \
          huffman/huffman_compress.c \
          huffman/huffman_decompress.c

OBJECTS = $(SOURCES:.c=.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean