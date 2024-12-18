/**
 * @file main.c
 * @brief Main program for the file compressor utility.
 *
 * This file contains the main function and handles the command-line interface
 * for the file compressor. It parses command-line arguments, determines the
 * compression/decompression mode, selects the appropriate algorithm, and
 * manages the overall program flow.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rle/rle.h"
#include "huffman/huffman.h"  // Include the Huffman header
#include "utils/bit_manipulation.h"

/**
 * @brief Prints the usage message for the program.
 *
 * @param program_name The name of the executable.
 */
void usage(char *program_name) {
    fprintf(stderr, "Usage: %s [-c|-d] [-a rle|huffman] input_file output_file\n", program_name);
    fprintf(stderr, "  -c : Compress\n");
    fprintf(stderr, "  -d : Decompress\n");
    fprintf(stderr, "  -a : Algorithm (rle or huffman, default: rle)\n");
    exit(1);
}

/**
 * @brief The main function of the file compressor program.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line argument strings.
 * @return int The exit status of the program (0 on success, 1 on error).
 */
int main(int argc, char *argv[]) {
    int compress_mode = -1;            // -1: unset, 0: decompress, 1: compress
    char *algorithm = "rle";           // Default algorithm
    char *input_filename = NULL;       // Input filename
    char *output_filename = NULL;      // Output filename

    // Check for minimum number of arguments
    if (argc < 4) {
        usage(argv[0]);
    }

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0) {
            if (compress_mode != -1) {
                usage(argv[0]); // -c or -d specified multiple times
            }
            compress_mode = 1;
        } else if (strcmp(argv[i], "-d") == 0) {
            if (compress_mode != -1) {
                usage(argv[0]); // -c or -d specified multiple times
            }
            compress_mode = 0;
        } else if (strcmp(argv[i], "-a") == 0) {
            if (i + 1 >= argc) {
                usage(argv[0]); // -a specified without an algorithm
            }
            algorithm = argv[++i];
        } else {
            // Positional arguments (filenames)
            if (!input_filename) {
                input_filename = argv[i];
            } else if (!output_filename) {
                output_filename = argv[i];
            } else {
                usage(argv[0]); // Too many positional arguments
            }
        }
    }

    // Check for mandatory options
    if (compress_mode == -1 || !input_filename || !output_filename) {
        usage(argv[0]); // Missing required arguments
    }

    // Validate algorithm
    if (strcmp(algorithm, "rle") != 0 && strcmp(algorithm, "huffman") != 0) {
        fprintf(stderr, "Invalid algorithm: %s\n", algorithm);
        usage(argv[0]);
    }

    // Open input and output files
    FILE *input_file = fopen(input_filename, "rb");
    if (!input_file) {
        perror("Error opening input file");
        return 1;
    }

    FILE *output_file = fopen(output_filename, "wb");
    if (!output_file) {
        perror("Error opening output file");
        fclose(input_file);
        return 1;
    }

    // Call appropriate compression/decompression function
    int result;
    if (strcmp(algorithm, "rle") == 0) {
        if (compress_mode == 1) {
            result = rle_compress(input_file, output_file);
        } else {
            result = rle_decompress(input_file, output_file);
        }
    } else if (strcmp(algorithm, "huffman") == 0) {  // Handle Huffman coding
        if (compress_mode == 1) {
            result = huffman_compress(input_file, output_file);
        } else {
            result = huffman_decompress(input_file, output_file);
        }
    } else {
        fprintf(stderr, "Invalid algorithm: %s\n", algorithm);
        usage(argv[0]);
    }

    // Handle result of compression/decompression
    if (result != 0) {
        fprintf(stderr, "Error during %s operation.\n", compress_mode == 1 ? "compression" : "decompression");
        // Clean up or additional error handling can be added here if necessary
    } else {
        printf("%s completed successfully.\n", compress_mode == 1 ? "Compression" : "Decompression");
    }

    // Close files and cleanup
    fclose(input_file);
    fclose(output_file);

    return result == 0 ? 0 : 1;
}