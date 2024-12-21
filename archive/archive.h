#ifndef ARCHIVE_H
#define ARCHIVE_H


#include <stdio.h>
#include "../reports/compression_report.h"


// Compresses an entire directory into an archive
// input_dir: Path to the directory to compress
// output_archive: Path to the output archive file
// algorithm: Compression algorithm to use (RLE, Huffman, or Hybrid)
// level: Compression intensity (fast, balanced, or maximum)
// Returns: 0 on success, -1 on error
int compress_directory(
    const char *input_dir,           // Input directory path
    const char *output_archive,      // Output archive file path
    CompressionAlgorithm algorithm,  // Compression algorithm type
    CompressionLevel level           // Compression intensity level
);


// Compresses multiple files into a single archive
// input_files: Array of file paths to compress
// file_count: Number of files in the input array
// output_archive: Path to the output archive file
// algorithm: Compression algorithm to use (RLE, Huffman, or Hybrid)
// level: Compression intensity (fast, balanced, or maximum)
// Returns: 0 on success, -1 on error
int compress_multiple_files(
    char **input_files,               // Array of input file paths
    int file_count,                   // Number of input files
    const char *output_archive,       // Output archive file path
    CompressionAlgorithm algorithm,   // Compression algorithm type
    CompressionLevel level            // Compression intensity level
);


#endif // ARCHIVE_H