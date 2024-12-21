#include "rle.h"
#include "../reports/compression_report.h"
#include <stdio.h>
#include <stdint.h>

#define BUFFER_SIZE 4096

// Define the ProgressCallback type
typedef void (*ProgressCallback)(size_t bytes_processed, size_t total_bytes, void *user_data);

/**
 * @brief Compresses the input file using the RLE algorithm.
 * 
 * @param input_file Input file to compress.
 * @param output_file File to write compressed data.
 * @return int 0 on success, -1 on error.
 */
int rle_compress(FILE *input_file, FILE *output_file) {
    if (input_file == NULL || output_file == NULL) {
        fprintf(stderr, "Invalid file pointers.\n");
        return -1;
    }

    uint8_t buffer[BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, input_file)) > 0) {
        size_t i = 0;
        while (i < bytes_read) {
            uint8_t current_byte = buffer[i];
            size_t count = 1;

            // Count consecutive occurrences
            while (i + count < bytes_read && buffer[i + count] == current_byte && count < 255) {
                count++;
            }

            // Write count and byte
            uint8_t compressed_data[2] = {count, current_byte};
            if (fwrite(compressed_data, 1, 2, output_file) != 2) {
                perror("Error writing compressed data");
                return -1;
            }

            i += count;
        }
    }

    if (ferror(input_file)) {
        perror("Error reading input file");
        return -1;
    }

    return 0;
}

/**
 * @brief Compresses the input file to the output file using RLE with specified compression level.
 * 
 * @param input_file Input file to compress.
 * @param output_file Output file to write compressed data.
 * @param level Compression level.
 * @return int 0 on success, -1 on error.
 */
int rle_compress_advanced(FILE *input_file, FILE *output_file, CompressionLevel level) {
    if (input_file == NULL || output_file == NULL) {
        fprintf(stderr, "Invalid file pointers.\n");
        return -1;
    }

    uint8_t buffer[BUFFER_SIZE];
    size_t bytes_read;

    // Adjust max_count based on compression level
    size_t max_count;
    switch (level) {
        case COMPRESSION_FAST:
            max_count = 64;  // Smaller count for faster compression
            break;
        case COMPRESSION_BALANCED:
            max_count = 128; // Balanced count
            break;
        case COMPRESSION_MAX:
            max_count = 255; // Maximum count for maximum compression
            break;
        default:
            max_count = 128; // Default to balanced
    }

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, input_file)) > 0) {
        size_t i = 0;
        while (i < bytes_read) {
            uint8_t current_byte = buffer[i];
            size_t count = 1;

            // Count consecutive occurrences
            while (i + count < bytes_read && buffer[i + count] == current_byte && count < max_count) {
                count++;
            }

            // Write count and byte
            uint8_t compressed_data[2] = {count, current_byte};
            if (fwrite(compressed_data, 1, 2, output_file) != 2) {
                perror("Error writing compressed data");
                return -1;
            }

            i += count;
        }
    }

    if (ferror(input_file)) {
        perror("Error reading input file");
        return -1;
    }

    return 0;
}

int rle_compress_with_progress(FILE *input_file, FILE *output_file, CompressionLevel level, ProgressCallback progress_fn, void *user_data) {
    if (input_file == NULL || output_file == NULL) {
        fprintf(stderr, "Invalid file pointers.\n");
        return -1;
    }

    // Get the total size of the input file for progress tracking
    fseek(input_file, 0, SEEK_END);
    size_t total_size = ftell(input_file);
    rewind(input_file);

    uint8_t buffer[BUFFER_SIZE];
    size_t bytes_read;
    size_t total_bytes_processed = 0;

    // Adjust max_count based on compression level
    size_t max_count;
    switch (level) {
        case COMPRESSION_FAST:
            max_count = 64;
            break;
        case COMPRESSION_BALANCED:
            max_count = 128;
            break;
        case COMPRESSION_MAX:
            max_count = 255;
            break;
        default:
            max_count = 128;
    }

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, input_file)) > 0) {
        size_t i = 0;
        while (i < bytes_read) {
            uint8_t current_byte = buffer[i];
            size_t count = 1;

            // Count consecutive occurrences
            while (i + count < bytes_read && buffer[i + count] == current_byte && count < max_count) {
                count++;
            }

            // Write count and byte
            uint8_t compressed_data[2] = {count, current_byte};
            if (fwrite(compressed_data, 1, 2, output_file) != 2) {
                perror("Error writing compressed data");
                return -1;
            }

            i += count;
            total_bytes_processed += count;
        }

        // Call the progress callback function
        if (progress_fn) {
            progress_fn(total_bytes_processed, total_size, user_data);
        }
    }

    if (ferror(input_file)) {
        perror("Error reading input file");
        return -1;
    }

    return 0;
}