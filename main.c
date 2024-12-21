#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "rle/rle.h"
#include "huffman/huffman.h"
#include "utils/bit_manipulation.h"
#include "reports/compression_report.h"
#include "archive/archive.h"
#include "benchmark/benchmark.h"
#include "encryption/encryption.h"
#include <unistd.h>
#include <limits.h>
#include <getopt.h>
#include <openssl/evp.h>
#include <ctype.h>

void my_progress_callback(size_t bytes_processed, size_t total_bytes, void *user_data) {
    if (total_bytes > 0) {
        int percentage = (int)((double)bytes_processed / total_bytes * 100);
        fprintf(stderr, "Compression/Decompression Progress: %d%%   \r", percentage);
    }
}

// Function to check if a file exists
int file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

// Function to copy a file (used for decompression after decryption)
int copy_file(FILE *source, FILE *destination) {
    char buffer[4096];
    size_t bytes;

    while ((bytes = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        fwrite(buffer, 1, bytes, destination);
    }

    return 0; // Assume success, add error handling if needed
}

/**
 * @brief Prints the usage message for the program.
 *
 * @param program_name The name of the executable.
 */
void usage(char *program_name) {
    fprintf(stderr, "Usage: %s [-c|-d|-b] [-a rle|huffman|hybrid] [-l fast|balanced|max] [-dir directory] [-files file1 file2 ...] [-encrypt|-decrypt] [-password password] input_file output_file\n", program_name);
    fprintf(stderr, "  -c                  : Compress. Compress an input file or directory.\n");
    fprintf(stderr, "  -d                  : Decompress. Decompress an input file.\n");
    fprintf(stderr, "  -b                  : Benchmark. Benchmark compression/decompression performance.\n");
    fprintf(stderr, "  -a                  : Algorithm. Specify the compression algorithm (rle, huffman, hybrid).\n");
    fprintf(stderr, "                      Default: rle\n");
    fprintf(stderr, "  -l                  : Compression level. Specify the compression level (fast, balanced, max).\n");
    fprintf(stderr, "                      Default: balanced\n");
    fprintf(stderr, "  -dir <directory>    : Compress a directory. Use with -c.\n");
    fprintf(stderr, "  -files <files...>   : Compress multiple files. Use with -c.\n");
    fprintf(stderr, "  -encrypt            : Encrypt the compressed file.\n");
    fprintf(stderr, "  -decrypt            : Decrypt the compressed file.\n");
    fprintf(stderr, "  -password <password>: Password. Provide a password for encryption or decryption.\n");
    fprintf(stderr, "  input_file          : Input file or directory for compression/decompression.\n");
    fprintf(stderr, "  output_file         : Output file for compressed or decompressed data.\n");

    exit(1);
}

// Function to convert string to lowercase
char* strtolower(char *str) {
    for (char *p = str; *p; p++) {
        *p = tolower(*p);
    }
    return str;
}

// New function to perform hybrid compression
int hybrid_compress(FILE *input_file, FILE *output_file, CompressionLevel level) {
    // Sample size for comparison (adjust as needed)
    const size_t sample_size = 1024;
    uint8_t sample[sample_size];

    // Read a sample from the input file
    size_t bytes_read = fread(sample, 1, sample_size, input_file);
    if (bytes_read == 0) {
        return -1; // Error reading or empty file
    }
    rewind(input_file);

    // Create temporary files for RLE and Huffman compression
    FILE *temp_rle = tmpfile();
    FILE *temp_huffman = tmpfile();
    if (!temp_rle || !temp_huffman) {
        perror("Error creating temporary files");
        return -1;
    }

    // Perform RLE compression on the sample
    size_t rle_compressed_size = 0;
    if (rle_compress_advanced(input_file, temp_rle, level) == 0)
    {
        // Get compressed size
        fseek(temp_rle, 0, SEEK_END);
        rle_compressed_size = ftell(temp_rle);
        rewind(temp_rle);
    }
    rewind(input_file);

    // Perform Huffman compression on the sample
    size_t huffman_compressed_size = 0;
    if (huffman_compress(input_file, temp_huffman) == 0)
    {
        // Get compressed size
        fseek(temp_huffman, 0, SEEK_END);
        huffman_compressed_size = ftell(temp_huffman);
        rewind(temp_huffman);
    }
    rewind(input_file);

    // Determine the more efficient algorithm
    CompressionAlgorithm chosen_algorithm;
    if (huffman_compressed_size == 0 || (rle_compressed_size > 0 && rle_compressed_size < huffman_compressed_size)) {
        chosen_algorithm = ALG_RLE;
        
        // Copy temp_rle to output_file
        uint8_t buffer[4096];
        size_t bytes;
        fseek(temp_rle, 0, SEEK_SET);
        while ((bytes = fread(buffer, 1, sizeof(buffer), temp_rle)) > 0) {
            fwrite(buffer, 1, bytes, output_file);
        }
        printf("RLE Algorithm is choosen by hybrid algorithm\n");
    } else {
        chosen_algorithm = ALG_HUFFMAN;

        // Copy temp_huffman to output_file
        uint8_t buffer[4096];
        size_t bytes;
        fseek(temp_huffman, 0, SEEK_SET);
        while ((bytes = fread(buffer, 1, sizeof(buffer), temp_huffman)) > 0) {
            fwrite(buffer, 1, bytes, output_file);
        }
        printf("Huffman Algorithm is choosen by hybrid algorithm\n");
    }

    // Clean up temporary files
    fclose(temp_rle);
    fclose(temp_huffman);

    return chosen_algorithm;
}

int main(int argc, char *argv[]) {
    int opt;
    int compress_mode = -1;  // -1: unset, 0: decompress, 1: compress, 2: benchmark
    char *algorithm = "rle";
    char *input_filename = NULL;
    char *output_filename = NULL;
    CompressionLevel level = COMPRESSION_BALANCED;
    char *dir_name = NULL;
    char **file_list = NULL;
    int file_count = 0;
    int encrypt = 0;
    int decrypt = 0;
    char *password = NULL;

    struct option long_options[] = {
        {"c", no_argument, NULL, 'c'},
        {"d", no_argument, NULL, 'd'},
        {"b", no_argument, NULL, 'b'},
        {"a", required_argument, NULL, 'a'},
        {"l", required_argument, NULL, 'l'},
        {"dir", required_argument, NULL, '1'},
        {"files", required_argument, NULL, '2'},
        {"encrypt", no_argument, &encrypt, 1},
        {"decrypt", no_argument, &decrypt, 1},
        {"password", required_argument, NULL, 'p'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "cdba:l:1:2:p:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'c':
                compress_mode = 1;
                break;
            case 'd':
                compress_mode = 0;
                break;
            case 'b':
                compress_mode = 2; // Benchmark mode
                break;
            case 'a':
                algorithm = strtolower(optarg);
                break;
            case 'l':
                if (strcmp(strtolower(optarg), "fast") == 0) {
                    level = COMPRESSION_FAST;
                } else if (strcmp(strtolower(optarg), "balanced") == 0) {
                    level = COMPRESSION_BALANCED;
                } else if (strcmp(strtolower(optarg), "max") == 0) {
                    level = COMPRESSION_MAX;
                } else {
                    fprintf(stderr, "Invalid compression level: %s\n", optarg);
                    usage(argv[0]);
                }
                break;
            case '1':
                dir_name = optarg;
                break;
            case '2':
                // Start adding file names after -files and continue until the next option
                file_list = &argv[optind - 1]; // Start of the file list
                file_count = 0;
                while (optind < argc && argv[optind][0] != '-') {
                    file_count++;
                    optind++;
                }
                break;
            case 'p':
                password = optarg;
                break;
            case 0:
                // For long options without a short equivalent
                break;
            case '?':
                usage(argv[0]);
                break;
        }
    }

    // Check if input and output files are provided correctly
    if (optind < argc) {
        input_filename = argv[optind++];
    }
    if (optind < argc) {
        output_filename = argv[optind++];
    }

    // If encrypt or decrypt is specified without a password, that's an error
    if ((encrypt || decrypt) && password == NULL) {
        fprintf(stderr, "Error: Encryption or decryption requested but no password provided.\n");
        usage(argv[0]);
    }

    if ((encrypt || decrypt) && file_count > 0) {
        fprintf(stderr, "Error: Encryption and decryption are not supported with multiple files.\n");
        usage(argv[0]);
    }

    if (compress_mode == -1) {
        fprintf(stderr, "Error: No operation specified.\n");
        usage(argv[0]);
    }

    if (compress_mode != 2 && (!input_filename || !output_filename)) {
        fprintf(stderr, "Error: Input and output filenames are required for compression/decompression.\n");
        usage(argv[0]);
    }

    // Check for algorithm validity, removed hybrid from decompress
    if (compress_mode == 0 && strcmp(algorithm, "hybrid") == 0) {
        fprintf(stderr, "Error: Hybrid mode is not supported for decompression.\n");
        usage(argv[0]);
    } else if (strcmp(algorithm, "rle") != 0 && strcmp(algorithm, "huffman") != 0 && strcmp(algorithm, "hybrid") != 0) {
        fprintf(stderr, "Error: Invalid algorithm specified.\n");
        usage(argv[0]);
    }

    CompressionReport report;
    int result = 0;
    // Encryption/decryption for single files
    if ((encrypt || decrypt) && file_count == 0 && !dir_name && input_filename && output_filename) {
        if (encrypt && !file_exists(input_filename)) {
            fprintf(stderr, "Error: Input file does not exist.\n");
            return 1;
        }

        FILE *input_file = fopen(input_filename, "rb");
        if (!input_file) {
            perror("Error opening input file");
            return 1;
        }

        // Use a temporary file for intermediate compressed data
        FILE *temp_compressed_file = tmpfile();
        if (!temp_compressed_file) {
            perror("Error creating temporary file for compression");
            fclose(input_file);
            return 1;
        }
        
        FILE *output_file;
        if (decrypt) {
            output_file = fopen(output_filename, "wb"); // Open for writing decompressed data
        } else {
            output_file = fopen(output_filename, "wb"); // Open for writing encrypted data
        }
        
        if (!output_file) {
            perror("Error opening output file");
            fclose(input_file);
            if (temp_compressed_file) {
                fclose(temp_compressed_file);
            }
            return 1;
        }
        
        if (encrypt)
        {
            if (algorithm == NULL || strcmp(algorithm, "rle") == 0)
            {
                if (rle_compress_advanced(input_file, temp_compressed_file, level) != 0)
                {
                    fprintf(stderr, "RLE compression failed.\n");
                    result = 1;
                }
            }
            else if (strcmp(algorithm, "huffman") == 0)
            {
                if (huffman_compress(input_file, temp_compressed_file) != 0)
                {
                    fprintf(stderr, "Huffman compression failed.\n");
                    result = 1;
                }
            } else if (strcmp(algorithm, "hybrid") == 0)
            {
                if (hybrid_compress(input_file, temp_compressed_file, level) < 0) {
                    fprintf(stderr, "Hybrid compression failed.\n");
                    result = 1;
                }
            }

            if (!result) {
                if (encrypt_compressed_file(temp_compressed_file, output_file, password) != 0)
                {
                    fprintf(stderr, "Encryption failed.\n");
                    result = 1;
                }
            }
        } else if (decrypt)
        {
            // Use a temporary file for intermediate compressed data
            FILE *temp_decrypted_file = tmpfile();
            if (!temp_decrypted_file) {
                perror("Error creating temporary file for compression");
                fclose(input_file);
                return 1;
            }
            
            if (decrypt_compressed_file(input_file, temp_decrypted_file, password) != 0)
            {
                fprintf(stderr, "Decryption failed.\n");
                result = 1;
            } else {
                rewind(temp_decrypted_file);
                
                if (algorithm == NULL || strcmp(algorithm, "rle") == 0)
                {
                    // Decompress from temp_decrypted_file to output_file
                    if (rle_decompress(temp_decrypted_file, output_file) != 0)
                    {
                        fprintf(stderr, "RLE decompression failed.\n");
                        result = 1;
                    }
                }
                else if (strcmp(algorithm, "huffman") == 0)
                {
                    // Decompress from temp_decrypted_file to output_file
                    if (huffman_decompress(temp_decrypted_file, output_file) != 0)
                    {
                        fprintf(stderr, "Huffman decompression failed.\n");
                        result = 1;
                    }
                }
            }
            if (temp_decrypted_file) fclose(temp_decrypted_file);
        }

        // Clean up
        fclose(input_file);
        fclose(output_file);
        if (temp_compressed_file) fclose(temp_compressed_file);

        // Handle result of encryption/decryption
        if (result != 0) {
            fprintf(stderr, "Error during %s operation.\n", encrypt ? "encryption" : "decryption");
        } else {
            printf("%s completed successfully.\n", encrypt ? "Encryption" : "Decryption");
        }

        return result;
    } else if (compress_mode == 1) { // Compression for files and directories
        // Compression
        FILE *output_file = fopen(output_filename, "wb");
        if (!output_file) {
            perror("Error opening output file");
            return 1;
        }

        if (file_count > 0) {
            // Compress multiple files
            if (strcmp(algorithm, "hybrid") == 0) {
                result = compress_multiple_files(file_list, file_count, output_filename, report.algorithm, level);
            } else {
                result = compress_multiple_files(file_list, file_count, output_filename, algorithm == NULL ? ALG_RLE : (strcmp(algorithm, "rle") == 0 ? ALG_RLE : ALG_HUFFMAN), level);
            }
            
            if (result != 0) {
                fprintf(stderr, "Error during multiple files compression.\n");
            } else {
                printf("Multiple files compression completed successfully.\n");
            }
        } else if (dir_name) {
            // Compress directory
            if (strcmp(algorithm, "hybrid") == 0) {
                result = compress_directory(dir_name, output_filename, report.algorithm, level);
            } else {
                result = compress_directory(dir_name, output_filename, algorithm == NULL ? ALG_RLE : (strcmp(algorithm, "rle") == 0 ? ALG_RLE : ALG_HUFFMAN), level);
            }

            if (result != 0) {
                fprintf(stderr, "Error during directory compression.\n");
            } else {
                printf("Directory compression completed successfully.\n");
            }
        } else {
            // Regular file compression
            FILE *input_file = fopen(input_filename, "rb");
            if (!input_file) {
                perror("Error opening input file");
                fclose(output_file);
                return 1;
            }

            // Initialize report
            memset(&report, 0, sizeof(CompressionReport));
            if (strcmp(algorithm, "rle") == 0) {
                report.algorithm = ALG_RLE;
            } else if (strcmp(algorithm, "huffman") == 0) {
                report.algorithm = ALG_HUFFMAN;
            } else if (strcmp(algorithm, "hybrid") == 0) {
                report.algorithm = ALG_HYBRID;
            }

            report.level = level;
            start_compression_timing(&report);

            // Perform compression based on the selected algorithm
            if (strcmp(algorithm, "rle") == 0) {
                result = rle_compress_with_progress(input_file, output_file, level, my_progress_callback, NULL);
            } else if (strcmp(algorithm, "huffman") == 0) {
                result = huffman_compress_with_progress(input_file, output_file, my_progress_callback, NULL);
            } else if (strcmp(algorithm, "hybrid") == 0) {
                result = hybrid_compress(input_file, output_file, level);
                if (result == ALG_RLE || result == ALG_HUFFMAN)
                {
                    report.algorithm = result;
                    result = 0; // Reset result to indicate success
                }
            }

            // Finalize report
            end_compression_timing(&report, input_file, output_file);
            
            if (result != 0) {
                fprintf(stderr, "Error during compression.\n");
            } else {
                printf("Compression completed successfully.\n");

                // Generate compression report
                FILE *report_file = fopen("compression_report.txt", "w");
                if (report_file) {
                    generate_compression_report(report_file, &report);
                    fclose(report_file);
                    printf("Compression report generated: compression_report.txt\n");
                } else {
                    perror("Error opening report file");
                }
            }

            fclose(input_file);
        }

        fclose(output_file);
    } else if (compress_mode == 0) {
        // Decompression (similar to your original logic)
        FILE *input_file = fopen(input_filename, "rb");
        FILE *output_file = fopen(output_filename, "wb");

        if (!input_file || !output_file) {
            perror("Error opening files");
            if (input_file) fclose(input_file);
            if (output_file) fclose(output_file);
            return 1;
        }

        if (strcmp(algorithm, "rle") == 0) {
            result = rle_decompress(input_file, output_file);
        } else if (strcmp(algorithm, "huffman") == 0) {
            result = huffman_decompress(input_file, output_file);
        } else {
            fprintf(stderr, "Error: Invalid algorithm specified for decompression.\n");
            result = 1;
        }

        if (result != 0) {
            fprintf(stderr, "Error during decompression.\n");
        } else {
            printf("Decompression completed successfully.\n");
        }

        fclose(input_file);
        fclose(output_file);
    } else if (compress_mode == 2) { // Benchmark mode
        // Validate algorithm and level if necessary
        if (strcmp(algorithm, "rle") != 0 && strcmp(algorithm, "huffman") != 0 && strcmp(algorithm, "hybrid") != 0) {
            fprintf(stderr, "Invalid algorithm for benchmark: %s\n", algorithm);
            return 1;
        }

        CompressionAlgorithm alg = ALG_RLE;
        if (strcmp(algorithm, "huffman") == 0) {
            alg = ALG_HUFFMAN;
        } else if (strcmp(algorithm, "hybrid") == 0) {
            alg = ALG_HYBRID;
        }

        CompressionBenchmark benchmark;
        result = benchmark_compression(input_filename, alg, level, &benchmark);

        if (result == 0) {
            printf("Benchmark results:\n");
            printf("Compression Time: %f seconds\n", benchmark.compression_time);
            printf("Decompression Time: %f seconds\n", benchmark.decompression_time);
            printf("CPU Usage: %f\n", benchmark.cpu_usage);
            printf("Memory Usage: %f KB\n", benchmark.memory_usage);
        } else {
            fprintf(stderr, "Benchmarking failed.\n");
        }
    }

    return result;
}