/**
 * @file utils/bit_manipulation.h
 * @brief Header file for bit manipulation utility functions.
 *
 * This file declares functions for manipulating individual bits within bytes,
 * including setting, clearing, getting, reading a bit from file and writing a
 * bit to a file and a special function to flush bits in the buffer to a file
 */

#ifndef BIT_MANIPULATION_H
#define BIT_MANIPULATION_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/**
 * @brief Sets a specific bit in a byte.
 *
 * @param byte Pointer to the byte where the bit will be set.
 * @param pos Position of the bit to set (0-7, where 0 is the least significant bit).
 */
void set_bit(uint8_t *byte, int pos);

/**
 * @brief Clears a specific bit in a byte.
 *
 * @param byte Pointer to the byte where the bit will be cleared.
 * @param pos Position of the bit to clear (0-7, where 0 is the least significant bit).
 */
void clear_bit(uint8_t *byte, int pos);

/**
 * @brief Gets the value of a specific bit in a byte.
 *
 * @param byte The byte from which to read the bit.
 * @param pos Position of the bit to get (0-7, where 0 is the least significant bit).
 * @return true if the bit is set (1), false if the bit is clear (0).
 */
bool get_bit(uint8_t byte, int pos);

/**
 * @brief Reads a single bit from a file.
 *
 * @param file Pointer to the file stream.
 * @param bit Pointer to store the bit that is read from the file.
 * @return true on success, false on EOF or error.
 */
bool read_bit(FILE *file, uint8_t *bit);

/**
 * @brief Writes a single bit to a file.
 *
 * @param file Pointer to the file stream.
 * @param bit The bit to write to the file (0 or 1).
 * @return int 0 on success, -1 on error.
 */
int write_bit(FILE *file, uint8_t bit);

/**
 * @brief Flush any remaining bits in the buffer to the file
 *
 * @param file Pointer to the file stream.
 * @return int 0 on success, -1 on error.
 */
int flush_bit_buffer(FILE *file);

#endif // BIT_MANIPULATION_H