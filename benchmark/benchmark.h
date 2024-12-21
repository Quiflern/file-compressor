#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "../reports/compression_report.h"

// Structure to hold compression benchmark results
typedef struct {
    double compression_time;
    double decompression_time;
    float cpu_usage;
    float memory_usage;
} CompressionBenchmark;

/**
 * @brief Benchmarks the compression and decompression performance.
 * 
 * @param input_filename Input file to use for benchmarking.
 * @param algorithm Compression algorithm to benchmark.
 * @param level Compression level (if applicable).
 * @param benchmark Pointer to store the benchmark results.
 * @return int 0 on success, -1 on error.
 */
int benchmark_compression(const char *input_filename, CompressionAlgorithm algorithm, CompressionLevel level, CompressionBenchmark *benchmark);

#endif // BENCHMARK_H