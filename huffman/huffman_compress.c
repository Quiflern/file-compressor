#include "huffman.h"
#include "../utils/bit_manipulation.h"
#include <stdlib.h>
#include <string.h>


// Utility function to create a new Huffman node
HuffmanNode* create_node(uint8_t character, unsigned frequency) {
    HuffmanNode* node = malloc(sizeof(HuffmanNode));
    if (!node) return NULL;
    
    node->character = character;
    node->frequency = frequency;
    node->left = NULL;
    node->right = NULL;
    
    return node;
}


// Free Huffman tree
void free_huffman_tree(HuffmanNode* node) {
    if (node == NULL) return;
    
    free_huffman_tree(node->left);
    free_huffman_tree(node->right);
    free(node);
}


// Compare function for sorting nodes
int compare_nodes(const void* a, const void* b) {
    HuffmanNode* node_a = *(HuffmanNode**)a;
    HuffmanNode* node_b = *(HuffmanNode**)b;
    return node_a->frequency - node_b->frequency;
}


// Build Huffman codes recursively
void build_huffman_codes(HuffmanNode* root, HuffmanCode* codes, 
                          uint32_t current_code, uint8_t code_length) {
    if (root == NULL) return;
    
    // Leaf node
    if (!root->left && !root->right) {
        codes[root->character].code = current_code;
        codes[root->character].code_length = code_length;
        return;
    }
    
    // Traverse left
    if (root->left) {
        build_huffman_codes(root->left, codes, 
                             (current_code << 1), 
                             code_length + 1);
    }
    
    // Traverse right
    if (root->right) {
        build_huffman_codes(root->right, codes, 
                             (current_code << 1) | 1, 
                             code_length + 1);
    }
}


// Build Huffman tree
HuffmanNode* build_huffman_tree(unsigned* frequencies) {
    HuffmanNode* nodes[MAX_CHARS];
    int node_count = 0;
    
    // Create nodes for characters with non-zero frequency
    for (int i = 0; i < MAX_CHARS; i++) {
        if (frequencies[i] > 0) {
            nodes[node_count++] = create_node(i, frequencies[i]);
        }
    }
    
    // Sort nodes by frequency
    for (int i = 0; i < node_count - 1; i++) {
        for (int j = 0; j < node_count - i - 1; j++) {
            if (nodes[j]->frequency > nodes[j + 1]->frequency) {
                HuffmanNode* temp = nodes[j];
                nodes[j] = nodes[j + 1];
                nodes[j + 1] = temp;
            }
        }
    }
    
    // Build tree
    while (node_count > 1) {
        HuffmanNode* left = nodes[0];
        HuffmanNode* right = nodes[1];
        
        HuffmanNode* parent = create_node(0, left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;
        
        // Remove first two nodes and insert parent
        for (int i = 2; i < node_count; i++) {
            nodes[i - 2] = nodes[i];
        }
        nodes[node_count - 2] = parent;
        node_count--;
    }
    
    return nodes[0];
}


int huffman_compress(FILE *input_file, FILE *output_file) {
    // Frequency calculation
    unsigned frequencies[MAX_CHARS] = {0};
    int c;
    size_t file_size = 0;
    
    // First pass: calculate frequencies
    while ((c = fgetc(input_file)) != EOF) {
        frequencies[c]++;
        file_size++;
    }
    rewind(input_file);
    
    // Build Huffman tree
    HuffmanNode* root = build_huffman_tree(frequencies);
    
    // Build Huffman codes
    HuffmanCode codes[MAX_CHARS] = {0};
    build_huffman_codes(root, codes, 0, 0);
    
    // Write file size and frequency table
    fwrite(&file_size, sizeof(size_t), 1, output_file);
    fwrite(frequencies, sizeof(unsigned), MAX_CHARS, output_file);
    
    // Compress file
    while ((c = fgetc(input_file)) != EOF) {
        HuffmanCode code = codes[c];
        
        // Write code bit by bit
        for (int i = code.code_length - 1; i >= 0; i--) {
            uint8_t bit = (code.code >> i) & 1;
            if (write_bit(output_file, bit) != 0) {
                free_huffman_tree(root);
                return -1;
            }
        }
    }
    
    // Flush remaining bits
    flush_bit_buffer(output_file);
    
    // Clean up
    free_huffman_tree(root);
    
    return 0;
}