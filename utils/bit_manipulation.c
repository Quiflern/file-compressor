/**
 * @file utils/bit_manipulation.c
 * @brief Implements bit manipulation utility functions.
 *
 * This file provides the implementation for functions declared in bit_manipulation.h.
 * It includes functions to set, clear, and get individual bits within a byte, as well as
 * read and write a bit to a file using internal buffer to enhance the performence by reducing the
 * number of I/O operation with the files
 */

#include "bit_manipulation.h"
#include <stdio.h>

// Static variables for bit buffering
static uint8_t bit_buffer = 0;  // Buffer to store bits for writing
static int bit_count = 0;      // Number of bits currently in the buffer

/**
 * @brief Sets a specific bit in a byte.
 *
 * @param byte Pointer to the byte where the bit will be set.
 * @param pos Position of the bit to set (0-7, where 0 is the least significant bit).
 */
void set_bit(uint8_t *byte, int pos)
{
    *byte |= (1 << pos);
}

/**
 * @brief Clears a specific bit in a byte.
 *
 * @param byte Pointer to the byte where the bit will be cleared.
 * @param pos Position of the bit to clear (0-7, where 0 is the least significant bit).
 */
void clear_bit(uint8_t *byte, int pos)
{
    *byte &= ~(1 << pos);
}

/**
 * @brief Gets the value of a specific bit in a byte.
 *
 * @param byte The byte from which to read the bit.
 * @param pos Position of the bit to get (0-7, where 0 is the least significant bit).
 * @return true if the bit is set (1), false if the bit is clear (0).
 */
bool get_bit(uint8_t byte, int pos)
{
    return (byte >> pos) & 1;
}

/**
 * @brief Reads a single bit from a file.
 *
 * @param file Pointer to the file stream.
 * @param bit Pointer to store the bit that is read from the file.
 * @return true on success, false on EOF or error.
 */
bool read_bit(FILE *file, uint8_t *bit)
{
    if (bit_count == 0)
    {
        // Fill the buffer
        if (fread(&bit_buffer, 1, 1, file) != 1)
        {
            return false; // Error or EOF
        }
        bit_count = 8;
    }

    *bit = get_bit(bit_buffer, 7); // Read the most significant bit
    bit_buffer <<= 1;             // Left-shift the buffer
    bit_count--;

    return true;
}

/**
 * @brief Writes a single bit to a file.
 *
 * @param file Pointer to the file stream.
 * @param bit The bit to write to the file (0 or 1).
 * @return int 0 on success, -1 on error.
 */
int write_bit(FILE *file, uint8_t bit)
{
    bit_buffer <<= 1;     // Make space for the new bit
    if (bit)
    {
        bit_buffer |= 1;  // Set the least significant bit if needed
    }
    bit_count++;

    if (bit_count == 8)
    {
        // Buffer is full, write it to the file
        if (fwrite(&bit_buffer, 1, 1, file) != 1)
        {
            return -1; // Error writing
        }
        bit_buffer = 0;
        bit_count = 0;
    }

    return 0; // Success
}

/**
 * @brief Flush any remaining bits in the buffer to the file
 *
 * @param file Pointer to the file stream.
 * @return int 0 on success, -1 on error.
 */
int flush_bit_buffer(FILE *file)
{
    if (bit_count > 0)
    {
        // Pad the buffer with zeros to make a full byte
        bit_buffer <<= (8 - bit_count);
        if (fwrite(&bit_buffer, 1, 1, file) != 1)
        {
            return -1; // Error writing
        }
        bit_buffer = 0;
        bit_count = 0;
    }

    return 0; // Success
}