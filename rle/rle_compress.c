#include "rle.h"
#include <stdio.h>
#include <stdint.h>


#define BUFFER_SIZE 4096


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