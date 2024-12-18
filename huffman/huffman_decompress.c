#include "huffman.h"
#include "../utils/bit_manipulation.h"
#include <stdlib.h>


int huffman_decompress(FILE *input_file, FILE *output_file) {
    // Read original file size
    size_t file_size;
    if (fread(&file_size, sizeof(size_t), 1, input_file) != 1) {
        fprintf(stderr, "Error reading file size\n");
        return -1;
    }
    
    // Read frequency table
    unsigned frequencies[MAX_CHARS] = {0};
    if (fread(frequencies, sizeof(unsigned), MAX_CHARS, input_file) != MAX_CHARS) {
        fprintf(stderr, "Error reading frequency table\n");
        return -1;
    }
    
    // Rebuild Huffman tree
    HuffmanNode* root = build_huffman_tree(frequencies);
    if (root == NULL) {
        fprintf(stderr, "Error rebuilding Huffman tree\n");
        return -1;
    }
    
    // Decompress
    HuffmanNode* current = root;
    size_t decoded_bytes = 0;
    uint8_t bit;
    
    while (decoded_bytes < file_size) {
        // Read a bit
        if (!read_bit(input_file, &bit)) {
            // End of file or error
            if (decoded_bytes < file_size) {
                fprintf(stderr, "Unexpected end of file during decompression\n");
                free_huffman_tree(root);
                return -1;
            }
            break;
        }
        
        // Traverse the tree based on the bit
        current = (bit == 0) ? current->left : current->right;
        
        // If we've reached a leaf node
        if (current->left == NULL && current->right == NULL) {
            // Write the character
            if (fputc(current->character, output_file) == EOF) {
                fprintf(stderr, "Error writing decompressed data\n");
                free_huffman_tree(root);
                return -1;
            }
            
            // Reset to root for next character
            current = root;
            decoded_bytes++;
        }
    }
    
    // Clean up
    free_huffman_tree(root);
    
    return 0;
}