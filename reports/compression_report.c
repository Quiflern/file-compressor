#include "compression_report.h"
#include <string.h>

// Define the global report variable
CompressionReport report = {0};

// Generates a detailed compression report.
// Returns 0 on success, -1 on failure.
int generate_compression_report(FILE *report_file, const CompressionReport *report) {
    if (report_file == NULL || report == NULL) {
        return -1;
    }

    // Map algorithm enum to string
    const char *algorithm_str;
    switch (report->algorithm) {
        case ALG_RLE:
            algorithm_str = "RLE";
            break;
        case ALG_HUFFMAN:
            algorithm_str = "Huffman";
            break;
        case ALG_HYBRID:
            algorithm_str = "Hybrid";
            break;
        default:
            algorithm_str = "Unknown";
    }

    // Map compression level enum to string
    const char *level_str;
    switch (report->level) {
        case COMPRESSION_FAST:
            level_str = "Fast";
            break;
        case COMPRESSION_BALANCED:
            level_str = "Balanced";
            break;
        case COMPRESSION_MAX:
            level_str = "Max";
            break;
        default:
            level_str = "Unknown";
    }

    // Calculate compression time
    double compression_time = (double)(report->end_time - report->start_time) / CLOCKS_PER_SEC;

    // Write the report
    fprintf(report_file, "Compression Report\n");
    fprintf(report_file, "------------------\n");
    fprintf(report_file, "Algorithm: %s\n", algorithm_str);
    fprintf(report_file, "Compression Level: %s\n", level_str);
    fprintf(report_file, "Original Size: %zu bytes\n", report->original_size);
    fprintf(report_file, "Compressed Size: %zu bytes\n", report->compressed_size);
    fprintf(report_file, "Compression Ratio: %.2f\n", report->compression_ratio);
    fprintf(report_file, "Compression Time: %.4f seconds\n", compression_time);

    return 0;
}

// Starts the compression timer.
void start_compression_timing(CompressionReport *report) {
    if (report == NULL) return;
    report->start_time = clock();
}

// Ends the compression timer and calculates statistics.
void end_compression_timing(CompressionReport *report, FILE *input_file, FILE *output_file) {
    if (report == NULL || input_file == NULL || output_file == NULL) return;

    report->end_time = clock();

    // Get original file size
    fseek(input_file, 0, SEEK_END);
    report->original_size = ftell(input_file);
    rewind(input_file);

    // Get compressed file size
    fseek(output_file, 0, SEEK_END);
    report->compressed_size = ftell(output_file);
    rewind(output_file);

    // Calculate compression ratio
    if (report->original_size > 0) {
        report->compression_ratio = (float)report->compressed_size / report->original_size;
    } else {
        report->compression_ratio = 0.0f;
    }
}