#include "rle.h"
#include <stdio.h>
#include <stdint.h>


#define BUFFER_SIZE 4096


int rle_decompress(FILE *input_file, FILE *output_file) {
    if (input_file == NULL || output_file == NULL) {
        fprintf(stderr, "Invalid file pointers.\n");
        return -1;
    }


    uint8_t count, byte;
    uint8_t buffer[BUFFER_SIZE];
    (void)buffer;


    while (fread(&count, 1, 1, input_file) == 1) {
        if (fread(&byte, 1, 1, input_file) != 1) {
            fprintf(stderr, "Unexpected end of input file.\n");
            return -1;
        }


        // Repeat the byte 'count' times
        for (uint8_t i = 0; i < count; i++) {
            if (fwrite(&byte, 1, 1, output_file) != 1) {
                perror("Error writing decompressed data");
                return -1;
            }
        }
    }


    if (ferror(input_file)) {
        perror("Error reading input file");
        return -1;
    }


    return 0;
}