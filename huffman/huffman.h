#ifndef HUFFMAN_H
#define HUFFMAN_H


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


#define MAX_CHARS 256
#define MAX_TREE_HEIGHT 256


// Huffman tree node structure
typedef struct HuffmanNode {
    uint8_t character;
    unsigned frequency;
    struct HuffmanNode *left;
    struct HuffmanNode *right;
} HuffmanNode;


// Huffman coding table entry
typedef struct {
    uint32_t code;       // Bit representation of the code
    uint8_t code_length; // Length of the code in bits
} HuffmanCode;


// Compression metadata structure
typedef struct {
    size_t original_file_size;
    size_t compressed_file_size;
} CompressionMetadata;


// Function prototypes
int huffman_compress(FILE *input_file, FILE *output_file);
int huffman_decompress(FILE *input_file, FILE *output_file);


// Utility functions
HuffmanNode* create_node(uint8_t character, unsigned frequency);
void free_huffman_tree(HuffmanNode* node);
void build_huffman_codes(HuffmanNode* root, HuffmanCode* codes, 
                          uint32_t current_code, uint8_t code_length);


// Add this line to declare the build_huffman_tree function
HuffmanNode* build_huffman_tree(unsigned* frequencies);


#endif // HUFFMAN_H