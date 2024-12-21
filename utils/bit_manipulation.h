#ifndef BIT_MANIPULATION_H
#define BIT_MANIPULATION_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// Sets a specific bit in a byte.
void set_bit(uint8_t *byte, int pos);

// Clears a specific bit in a byte.
void clear_bit(uint8_t *byte, int pos);

// Gets the value of a specific bit in a byte.
// Returns true if the bit is set (1), false if the bit is clear (0).
bool get_bit(uint8_t byte, int pos);

// Reads a single bit from a file.
// Returns true on success, false on EOF or error.
bool read_bit(FILE *file, uint8_t *bit);

// Writes a single bit to a file.
// Returns 0 on success, -1 on error.
int write_bit(FILE *file, uint8_t bit);

// Flush any remaining bits in the buffer to the file.
// Returns 0 on success, -1 on error.
int flush_bit_buffer(FILE *file);

#endif // BIT_MANIPULATION_H