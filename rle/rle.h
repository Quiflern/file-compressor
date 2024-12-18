#ifndef RLE_H
#define RLE_H

#include <stdio.h>
#include <stdint.h>

// Function to compress data from an input file and writes the RLE compressed data to an output file.
int rle_compress(FILE *input_file, FILE *output_file);

// Function to decompress data from an input file and writes the RLE decompressed data to an output file.
int rle_decompress(FILE *input_file, FILE *output_file);

#endif // RLE_H