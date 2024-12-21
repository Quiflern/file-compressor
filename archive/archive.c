#include "archive.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "../rle/rle.h"
#include "../huffman/huffman.h"
#include "../reports/compression_report.h"
#include "../utils/bit_manipulation.h"
#include <limits.h>

// Fallback definition if not provided by system headers
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

// External report variable
extern CompressionReport report;

// Compresses input to output with the given compression level. Returns 0 on success, < 0 on error.
int hybrid_compress(FILE *input, FILE *output, int level);

// Define a structure for file metadata
typedef struct {
    char filepath[PATH_MAX];
    off_t size;
    mode_t mode;
    time_t mtime;
} FileMetadata;

// Function to compress a single file
int compress_single_file(const char *input_file, FILE *output_file, CompressionAlgorithm algorithm, CompressionLevel level) {
    FILE *in_file = fopen(input_file, "rb");
    if (!in_file) {
        perror("Error opening input file");
        return -1;
    }

    // Determine the appropriate compression function based on the algorithm
    int (*compress_func)(FILE *, FILE *, CompressionLevel) = NULL;
    if (algorithm == ALG_RLE) {
        compress_func = rle_compress_advanced;
    } else if (algorithm == ALG_HUFFMAN) {
        compress_func = (int (*)(FILE *, FILE *, CompressionLevel))huffman_compress;
    }
    // Hybrid is not supported for single file compression at this stage.

    if (compress_func) {
        // Call the appropriate compression function with the selected level
        if (compress_func(in_file, output_file, level) != 0) {
            fprintf(stderr, "Error during compression.\n");
            fclose(in_file);
            return -1;
        }
    } else {
        fprintf(stderr, "Unsupported compression algorithm.\n");
        fclose(in_file);
        return -1;
    }

    fclose(in_file);
    return 0;
}

// Function to write file metadata to archive
int write_file_metadata(FILE *archive, const FileMetadata *metadata) {
    if (fwrite(metadata, sizeof(FileMetadata), 1, archive) != 1) {
        perror("Error writing file metadata");
        return -1;
    }
    return 0;
}

// Function to read file metadata from archive
int read_file_metadata(FILE *archive, FileMetadata *metadata) {
    if (fread(metadata, sizeof(FileMetadata), 1, archive) != 1) {
        if (feof(archive)) {
            return 0; // End of archive
        }
        perror("Error reading file metadata");
        return -1;
    }
    return 1;
}

int compress_directory(const char *input_dir, const char *output_archive, CompressionAlgorithm algorithm, CompressionLevel level) {
    // Open the output archive file
    FILE *archive_file = fopen(output_archive, "wb");
    if (!archive_file) {
        perror("Error opening output archive file");
        return -1;
    }

    // Open the input directory
    DIR *dir = opendir(input_dir);
    if (!dir) {
        perror("Error opening input directory");
        fclose(archive_file);
        return -1;
    }

    struct dirent *entry;
    char filepath[PATH_MAX];
    struct stat file_stat;

    // Traverse the directory
    while ((entry = readdir(dir)) != NULL) {
        // Skip '.' and '..'
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construct the full file path
        snprintf(filepath, sizeof(filepath), "%s/%s", input_dir, entry->d_name);

        // Get file information
        if (stat(filepath, &file_stat) < 0) {
            perror("Error getting file information");
            continue;
        }

        // If it's a directory, call recursively
        if (S_ISDIR(file_stat.st_mode)) {
            compress_directory(filepath, output_archive, algorithm, level);
        } else if (S_ISREG(file_stat.st_mode)) {
            // Compress and add the file to the archive
            FileMetadata metadata;
            strncpy(metadata.filepath, filepath, sizeof(metadata.filepath));
            metadata.size = file_stat.st_size;
            metadata.mode = file_stat.st_mode;
            metadata.mtime = file_stat.st_mtime;

            // Write metadata
            if (write_file_metadata(archive_file, &metadata) != 0) {
                fprintf(stderr, "Failed to write metadata for %s\n", filepath);
                continue;
            }

            // Compress the file and write its content to the archive
            FILE *temp_file = tmpfile();
            if (!temp_file) {
                perror("Error creating temporary file for compression");
                continue;
            }

            // Use the chosen compression algorithm
            if (algorithm == ALG_RLE) {
                if (rle_compress_advanced(fopen(filepath, "rb"), temp_file, level) != 0) {
                    fprintf(stderr, "Error during RLE compression of %s\n", filepath);
                    fclose(temp_file);
                    continue;
                }
            } else if (algorithm == ALG_HUFFMAN) {
                if (huffman_compress(fopen(filepath, "rb"), temp_file) != 0) {
                    fprintf(stderr, "Error during Huffman compression of %s\n", filepath);
                    fclose(temp_file);
                    continue;
                }
            }

            // Write the compressed data to the archive
            rewind(temp_file);
            char buffer[4096];
            size_t bytes_read;
            while ((bytes_read = fread(buffer, 1, sizeof(buffer), temp_file)) > 0) {
                if (fwrite(buffer, 1, bytes_read, archive_file) != bytes_read) {
                    perror("Error writing compressed data to archive");
                    break;
                }
            }

            fclose(temp_file);
        }
    }

    closedir(dir);
    fclose(archive_file);
    return 0;
}

int compress_multiple_files(char **input_files, int file_count, const char *output_archive, CompressionAlgorithm algorithm, CompressionLevel level) {
    // Open the output archive file
    FILE *archive_file = fopen(output_archive, "wb");
    if (!archive_file) {
        perror("Error opening output archive file");
        return -1;
    }

    struct stat file_stat;
    for (int i = 0; i < file_count; i++) {
        // Get file information
        if (stat(input_files[i], &file_stat) < 0) {
            perror("Error getting file information");
            continue;
        }

        // Add the file to the archive
        FileMetadata metadata;
        strncpy(metadata.filepath, input_files[i], sizeof(metadata.filepath));
        metadata.size = file_stat.st_size;
        metadata.mode = file_stat.st_mode;
        metadata.mtime = file_stat.st_mtime;

        // Write metadata
        if (write_file_metadata(archive_file, &metadata) != 0) {
            fprintf(stderr, "Failed to write metadata for %s\n", input_files[i]);
            continue;
        }

        // Compress the file and write its content to the archive
        FILE *temp_file = tmpfile();
        if (!temp_file) {
            perror("Error creating temporary file for compression");
            continue;
        }

        // Compress the file using the appropriate algorithm
        if (algorithm == ALG_RLE) {
            if (rle_compress_advanced(fopen(input_files[i], "rb"), temp_file, level) != 0) {
                fprintf(stderr, "Error during RLE compression of %s\n", input_files[i]);
                fclose(temp_file);
                continue;
            }
        } else if (algorithm == ALG_HUFFMAN) {
            if (huffman_compress(fopen(input_files[i], "rb"), temp_file) != 0) {
                fprintf(stderr, "Error during Huffman compression of %s\n", input_files[i]);
                fclose(temp_file);
                continue;
            }
        } else if (algorithm == ALG_HYBRID) {
            // For hybrid, compress to a temporary file first to decide which algorithm to use
            FILE *temp_hybrid = tmpfile();
            if (!temp_hybrid) {
                perror("Error creating temporary file for hybrid compression");
                fclose(temp_file);
                continue;
            }
            if (hybrid_compress(fopen(input_files[i], "rb"), temp_hybrid, level) < 0) {
                fprintf(stderr, "Error during Hybrid compression decision for %s\n", input_files[i]);
                fclose(temp_hybrid);
                fclose(temp_file);
                continue;
            }
            
            // Use the algorithm chosen by hybrid_compress
            if (report.algorithm == ALG_RLE) {
                if (rle_compress_advanced(fopen(input_files[i], "rb"), temp_file, level) != 0) {
                    fprintf(stderr, "Error during RLE compression of %s\n", input_files[i]);
                    fclose(temp_hybrid);
                    fclose(temp_file);
                    continue;
                }
            } else if (report.algorithm == ALG_HUFFMAN) {
                if (huffman_compress(fopen(input_files[i], "rb"), temp_file) != 0) {
                    fprintf(stderr, "Error during Huffman compression of %s\n", input_files[i]);
                    fclose(temp_hybrid);
                    fclose(temp_file);
                    continue;
                }
            }
        }
            

        // Write the compressed data to the archive
        rewind(temp_file);
        char buffer[4096];
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), temp_file)) > 0) {
            if (fwrite(buffer, 1, bytes_read, archive_file) != bytes_read) {
                perror("Error writing compressed data to archive");
                break;
            }
        }

        fclose(temp_file);
    }

    fclose(archive_file);
    return 0;
}