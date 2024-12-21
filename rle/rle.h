#ifndef RLE_H
#define RLE_H

#include <stdio.h>
#include <stdint.h>
#include "../reports/compression_report.h"

// Function to compress data from an input file and write the RLE compressed data to an output file.
int rle_compress(FILE *input_file, FILE *output_file);

// Function to decompress data from an input file and write the RLE decompressed data to an output file.
int rle_decompress(FILE *input_file, FILE *output_file);

// Advanced RLE compression function with compression levels
int rle_compress_advanced(FILE *input_file, FILE *output_file, CompressionLevel level);

// Define the ProgressCallback type
typedef void (*ProgressCallback)(size_t bytes_processed, size_t total_bytes, void *user_data);

// RLE compression function with progress tracking
int rle_compress_with_progress(FILE *input_file, FILE *output_file, CompressionLevel level, ProgressCallback progress_fn, void *user_data);

#endif // RLE_H