#ifndef COMPRESSION_REPORT_H
#define COMPRESSION_REPORT_H

#include <stdio.h>
#include <time.h>

// Compression algorithm type
typedef enum {
    ALG_RLE,
    ALG_HUFFMAN,
    ALG_HYBRID
} CompressionAlgorithm;

// Compression level type
typedef enum {
    COMPRESSION_FAST,      // Prioritize speed
    COMPRESSION_BALANCED,  // Balance between speed and compression
    COMPRESSION_MAX        // Maximum compression ratio
} CompressionLevel;

// Structure to hold compression metadata and statistics
typedef struct {
    CompressionAlgorithm algorithm;
    CompressionLevel level;
    size_t original_size;
    size_t compressed_size;
    float compression_ratio;
    clock_t start_time;
    clock_t end_time;
} CompressionReport;

// External report variable
extern CompressionReport report;

// Function to generate a detailed compression report
int generate_compression_report(FILE *report_file, const CompressionReport *report);

// Function to start compression timer
void start_compression_timing(CompressionReport *report);

// Function to end compression timer and calculate statistics
void end_compression_timing(CompressionReport *report, FILE *input_file, FILE *output_file);

#endif // COMPRESSION_REPORT_H