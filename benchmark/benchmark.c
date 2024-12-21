#include "benchmark.h"
#include "../reports/compression_report.h"
#include "../rle/rle.h"
#include "../huffman/huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/resource.h>

// External report variable
extern CompressionReport report;

// Compresses input to output with the given compression level. Returns 0 on success, < 0 on error.
int hybrid_compress(FILE *input, FILE *output, int level);

// Helper function to get CPU time
double get_cpu_time() {
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        return (double)usage.ru_utime.tv_sec + (double)usage.ru_utime.tv_usec / 1000000.0;
    } else {
        return 0.0;
    }
}

// Helper function to get memory usage (resident set size)
long get_memory_usage() {
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        return usage.ru_maxrss; // in kilobytes
    } else {
        return 0;
    }
}

int benchmark_compression(const char *input_filename, CompressionAlgorithm algorithm, CompressionLevel level, CompressionBenchmark *benchmark) {
    if (!benchmark) return -1;

    FILE *input_file = fopen(input_filename, "rb");
    if (!input_file) {
        perror("Error opening input file for benchmarking");
        return -1;
    }

    // Create a temporary file for compressed output
    FILE *compressed_file = tmpfile();
    if (!compressed_file) {
        perror("Error creating temporary file for compression");
        fclose(input_file);
        return -1;
    }

    double start_cpu = get_cpu_time();
    long start_memory = get_memory_usage();
    clock_t start_time = clock();

    // Perform compression
    int compression_result = -1;
    if (algorithm == ALG_RLE) {
        compression_result = rle_compress_advanced(input_file, compressed_file, level);
    } else if (algorithm == ALG_HUFFMAN) {
        compression_result = huffman_compress(input_file, compressed_file);
    } else if (algorithm == ALG_HYBRID) {
        compression_result = hybrid_compress(input_file, compressed_file, level);
    }

    if (compression_result != 0) {
        fprintf(stderr, "Error during compression in benchmark\n");
        fclose(input_file);
        fclose(compressed_file);
        return -1;
    }

    clock_t end_time = clock();
    double end_cpu = get_cpu_time();
    long end_memory = get_memory_usage();

    benchmark->compression_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    benchmark->cpu_usage = (float)(end_cpu - start_cpu);
    benchmark->memory_usage = (float)(end_memory - start_memory);

    // Rewind compressed file for decompression
    rewind(compressed_file);

    // Create a temporary file for decompressed output
    FILE *decompressed_file = tmpfile();
    if (!decompressed_file) {
        perror("Error creating temporary file for decompression");
        fclose(input_file);
        fclose(compressed_file);
        return -1;
    }

    start_time = clock();

    // Perform decompression
    int decompression_result = -1;
    if (algorithm == ALG_RLE) {
        decompression_result = rle_decompress(compressed_file, decompressed_file);
    } else if (algorithm == ALG_HUFFMAN) {
        decompression_result = huffman_decompress(compressed_file, decompressed_file);
    } else if (algorithm == ALG_HYBRID) {
        // Hybrid decompression is based on what was determined during compression
        if (report.algorithm == ALG_RLE) {
            decompression_result = rle_decompress(compressed_file, decompressed_file);
        } else if (report.algorithm == ALG_HUFFMAN) {
            decompression_result = huffman_decompress(compressed_file, decompressed_file);
        }
    }
        if (decompression_result != 0) {
        fprintf(stderr, "Error during decompression in benchmark\n");
        fclose(input_file);
        fclose(compressed_file);
        fclose(decompressed_file);
        return -1;
    }

    end_time = clock();
    benchmark->decompression_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // Clean up
    fclose(input_file);
    fclose(compressed_file);
    fclose(decompressed_file);

    return 0;
}