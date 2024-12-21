# File Compressor
![compressor](https://github.com/user-attachments/assets/1c1a896c-63fe-4605-b055-75ef316aabd8)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## Description

This project is a command-line file compression and decompression utility implemented in C. It provides a range of features for compressing and decompressing files and directories, including support for multiple algorithms, advanced file handling, encryption, and performance benchmarking. It utilizes two lossless data compression algorithms:

1. **Run-Length Encoding (RLE):** A straightforward algorithm that replaces consecutive repeating characters with a count and the character.
2. **Huffman Coding:** A more advanced algorithm that assigns variable-length codes to characters based on their frequency of occurrence, resulting in shorter codes for frequently used characters.
3. **Hybrid:** A intelligent algorithm that utilizes both RLE & Huffman by comparing on small portion of file & processing further using most efficient algorithm

The utility allows you to select either algorithm for both compression and decompression, offering flexibility and the potential for improved compression ratios, especially with Huffman coding for suitable file types.

**Features:**

- **Multiple Compression Algorithms:**
  - Run-Length Encoding (RLE)
  - Huffman Coding
  - Hybrid (selects between RLE and Huffman based on an initial assessment)
- **Advanced File Handling:**
  - Compress entire directories (preserving the structure).
  - Compress multiple files into a single archive.
- **Progress Tracking:**
  - Displays a progress bar during compression and decompression.
- **Encryption:**
  - Encrypt compressed files with AES-256 using a password.
  - Decrypt encrypted files.
- **Benchmarking:**
  - Measure compression/decompression time, CPU usage, and memory usage.
- **Compression Level:**
  - Fast, Balanced, Max (affects the trade off between speed & compression ratio in RLE & Hybrid mode)

## Project Structure

```
file-compressor/
├── archive/              # Functions for creating and managing archives
│   ├── archive.c         # Directory and multi-file compression logic
│   └── archive.h         # Header file for archive functions
├── benchmark/            # Benchmarking functions
│   ├── benchmark.c       # Compression performance measurement
│   └── benchmark.h       # Header file for benchmarking
├── encryption/           # Encryption and decryption functions
│   ├── encryption.c      # File encryption/decryption using AES-256
│   └── encryption.h      # Header file for encryption
├── huffman/              # Huffman coding implementation
│   ├── huffman.h         # Header file for Huffman functions
│   ├── huffman_compress.c # Huffman compression algorithm
│   └── huffman_decompress.c # Huffman decompression algorithm
├── rle/                  # Run-Length Encoding implementation
│   ├── rle.h             # Header file for RLE functions
│   ├── rle_compress.c    # RLE compression algorithm
│   └── rle_decompress.c  # RLE decompression algorithm
├── reports/              # Compression reporting functions
│   ├── compression_report.c # Generates compression reports
│   └── compression_report.h # Header file for compression report
├── utils/                # Utility functions
│   ├── bit_manipulation.c# Bit-level operation functions
│   └── bit_manipulation.h# Header for bit manipulation
├── LICENSE               # Project license (MIT)
├── Makefile              # Makefile for project compilation
└── main.c                # Main program & command-line interface
```

## Getting Started

These instructions will guide you through setting up the project, building the executable, and running the file compressor on your local machine.

### Prerequisites

Before you begin, ensure you have the following software installed on your system:

- **C Compiler:** A standard C compiler, such as GCC (GNU Compiler Collection).
- **Make:** The `make` utility to automate the compilation process using the provided Makefile.
- **OpenSSL:** The OpenSSL library is required for encryption and decryption functionality (specifically, for the `libcrypto` component).

You can typically install these tools through your operating system's package manager. For example:

- **Debian/Ubuntu:** `sudo apt-get install build-essential libssl-dev`
- **macOS:** Xcode Command Line Tools (which include `make` and `gcc`). You may need to install OpenSSL separately using a package manager like Homebrew: `brew install openssl`. Also, you might need to add `LDFLAGS += -L/usr/local/opt/openssl/lib` and `CFLAGS += -I/usr/local/opt/openssl/include` to the Makefile, after you installed OpenSSL.
- **Fedora/CentOS/RHEL:** `sudo dnf groupinstall "Development Tools" && sudo dnf install openssl-devel`
- **Windows:** You can use MinGW or Cygwin to get a GCC, Make & OpenSSL development environment on Windows.

### Cloning the Repository

1. Open your terminal or command prompt.
2. Navigate to the directory where you want to store the project.
3. Clone the repository from GitHub using the following command:

   ```bash
   git clone https://github.com/Quiflern/file-compressor.git
   ```

### Building the Executable

1. Navigate to the project's root directory (`file-compressor`):

   ```bash
   cd file-compressor
   ```

2. Build the executable using the `make` command:

   ```bash
   make
   ```

   This will create an executable file named `compressor` in the project directory.

### Running the Compressor

Now that you have built the executable, you can run it to compress or decompress files.

#### Usage

```bash
./compressor [-c|-d|-b] [-a rle|huffman|hybrid] [-l fast|balanced|max] [-dir directory] [-files file1 file2 ...] [-encrypt|-decrypt] [-password password] input_file output_file
```

#### Arguments

- **`-c`:** Indicates compression mode.
- **`-d`:** Indicates decompression mode.
- **`-b`:** Indicates benchmark mode (measures compression/decompression performance).
- **`-a [rle|huffman|hybrid]`:** Specifies the compression algorithm:
  - `rle`: Use Run-Length Encoding.
  - `huffman`: Use Huffman Coding.
  - `hybrid`: Intelligently selects between RLE and Huffman (default for compression).
  - **Default (decompression):** `rle` is used if the `-a` flag is omitted. For `-c`, `hybrid` is used by default if the flag is omitted.
- **`-l [fast|balanced|max]`:** Specifies the compression level (only relevant for RLE and hybrid algorithms):
  - `fast`: Prioritizes compression speed.
  - `balanced`: Balances speed and compression ratio.
  - `max`: Achieves maximum compression (may be slower).
  - **Default:** `balanced` is used if the `-l` flag is omitted.
- **`-dir directory`:** Compresses the specified directory (using compression mode).
- **`-files file1 file2 ...`:** Compresses multiple files into a single archive (using compression mode).
- **`-encrypt`:** Encrypt the compressed data using a password.
- **`-decrypt`:** Decrypt the encrypted file using a password.
- **`-password password`:** The password for encryption or decryption.
- **`input_file`**: The path to the file you want to compress, decompress or benchmark.
- **`output_file`**: The desired path for the output file.

#### Examples

1. ***Compress a file using RLE (balanced level):***

   ```bash
   ./compressor -c -a rle -l balanced input.txt output.rle
   ```

2. ***Decompress an RLE compressed file:***

   ```bash
   ./compressor -d -a rle output.rle decompressed.txt
   ```

3. ***Compress a file using Huffman (max level):***

   ```bash
   ./compressor -c -a huffman -l max input.txt output.huff
   ```

4. ***Decompress a Huffman file:***

   ```bash
   ./compressor -d -a huffman output.huff decompressed.txt
   ```

5. ***Compress a directory using hybrid algorithm (fast level):***

   ```bash
   ./compressor -c -a hybrid -l fast -dir my_directory output.archive
   ```

6. ***Compress multiple files using RLE:***

   ```bash
   ./compressor -c -a rle -files file1.txt file2.txt file3.txt output.archive
   ```

7. ***Encrypt a compressed file:***

   ```bash
   ./compressor -c -a rle -encrypt -password "yourpassword" input.txt output.rle.encrypted
   ```

8. ***Decrypt and decompress a file:***

   ```bash
   ./compressor -d -a rle -decrypt -password "yourpassword" output.rle.encrypted decompressed.txt
   ```

9. ***Benchmark RLE compression on a file:***

   ```bash
   ./compressor -b -a rle input.txt
   ```

## Algorithm Details

### Run-Length Encoding (RLE)

Run-Length Encoding is most effective for data with long sequences of repeating characters (e.g., images with solid color areas, text with many repeated spaces). It works by:

1. Reading consecutive characters from the input file.
2. Counting the number of times a character repeats consecutively.
3. Storing the count and the character in the compressed file.

**Implementation Files:**

- **`rle/rle.h`**: Declares the RLE compression and decompression functions (`rle_compress` and `rle_decompress`).
- **`rle/rle_compress.c`**: Contains the implementation of RLE compression logic.
- **`rle/rle_decompress.c`**: Contains the implementation of RLE decompression logic.

### Huffman Coding

Huffman coding is generally more effective than RLE for a wider range of data, as it takes advantage of character frequencies. Here's how it works:

1. **Frequency Analysis:** It calculates the frequency of each character in the input file.
2. **Huffman Tree Construction:** Builds a binary tree where:

   - Leaves represent characters.
   - Paths from the root to leaves represent codes (shorter paths for frequent characters).
   - The tree is constructed in a way that ensures unique decodability.

3. **Code Assignment:** Assigns variable-length binary codes to characters based on the tree.
4. **Encoding:** Replaces each character in the input file with its assigned Huffman code.

**Implementation Files:**

- **`huffman/huffman.h`**: Declares Huffman coding-related functions and data structures (e.g., `HuffmanNode`, `huffman_compress`, `huffman_decompress`).
- **`huffman/huffman_compress.c`**: Implements Huffman compression, including frequency analysis, Huffman tree construction, code generation, and encoding.
- **`huffman/huffman_decompress.c`**: Implements Huffman decompression, including reading the frequency table, rebuilding the Huffman tree, and decoding.

### Hybrid Algorithm

The hybrid algorithm combines the strengths of both RLE and Huffman coding to potentially achieve better compression ratios. It operates as follows:

1. **Sample Analysis:** The algorithm first reads a small sample of data from the beginning of the input file.
2. **Compression Trial:** It attempts to compress the sample data using both RLE and Huffman coding.
3. **Algorithm Selection:** Based on which algorithm achieves better compression on the sample data, the hybrid algorithm selects either RLE or Huffman to compress the entire input file.

**Implementation Files:**

- **`main.c`**: The hybrid compression logic is primarily implemented within the `main` function, utilizing the RLE and Huffman compression functions as needed.

### Progress Tracking

The progress tracking feature in the compressor utility allows you to monitor the progress of compression and decompression operations. It displays the percentage of data processed so far in real-time, providing feedback on the ongoing operation. Here's how it works:

- During compression and decompression, the utility periodically calculates the number of bytes processed out of the total file size.
- It then uses a callback function to update the progress status to the user. The `my_progress_callback` function in main.c is an example of such a callback.
- The progress is printed to the standard error stream (stderr), overwriting the previous progress message. This creates a dynamic progress bar effect on the console.

**Implementation Details:**

- The callback function `my_progress_callback` is defined in `main.c` and is set up to be called by the compression or decompression functions.
- Functions like `rle_compress_with_progress` and `huffman_compress_with_progress` are modified versions of the compression functions that incorporate this progress reporting.
- For RLE, progress tracking is straightforward as the compression/decompression is done byte by byte.
- For Huffman coding, the progress is reported both during the frequency calculation phase and the actual encoding phase.

### Encryption

The compressor utility supports file encryption using the AES-256-CBC (Advanced Encryption Standard with 256-bit keys in Cipher Block Chaining mode) algorithm to secure compressed data. It allows you to compress a file and then encrypt it with a password. The decryption process reverses these steps, first decrypting the file and then decompressing it. Here's a closer look at the encryption and decryption features:

- **AES-256-CBC:** A robust and widely used symmetric encryption algorithm, meaning it uses the same key for both encryption and decryption.
- **Key Derivation:** The utility employs PBKDF2 (Password-Based Key Derivation Function 2) to derive a strong encryption key from the user-provided password.
- **Initialization Vector (IV):** A unique, random IV is generated for each encryption operation. The IV is essential for security in CBC mode and is stored along with the encrypted data.
- **Salt:** A random salt is used in the key derivation process. This makes it significantly harder for attackers to use precomputed rainbow tables to crack passwords.

**Implementation Files:**

- `encryption/encryption.h`: Declares the encryption and decryption functions (`encrypt_compressed_file` and `decrypt_compressed_file`).
- `encryption/encryption.c`: Implements the encryption and decryption logic, including key derivation, AES encryption/decryption, and handling of IV and salt.

**Usage:**

- **Encryption:** To encrypt a compressed file, use the `-encrypt` flag along with a password provided via the `-password` flag. The encryption is performed after compression. 
***Usage example:***
	```bash
	./compressor -c -a rle -encrypt -password "yourpassword" input.txt output.rle.encrypted
	```
- **Decryption:** To decrypt an encrypted file, use the `-decrypt` flag along with the correct password. The decryption is performed before decompression.
***Usage example:***
	``` bash
	./compressor -d -a rle -decrypt -password "yourpassword" output.rle.encrypted decompressed.txt
	```
**Security Considerations:**

- **Password Strength:** It is crucial to use a strong and unique password for encryption.
- **OpenSSL Dependency:** The encryption feature relies on the OpenSSL library, which must be installed on your system.

### Benchmark

The benchmark feature in the compressor utility is designed to provide insights into the performance of the compression and decompression processes. It allows you to measure and evaluate key performance metrics, such as:

- **Compression Time:** The time taken to compress a file using a specified algorithm and compression level.
- **Decompression Time:** The time taken to decompress a previously compressed file.
- **CPU Usage:** The amount of CPU time consumed during the compression and decompression operations.
- **Memory Usage:** The peak amount of memory (resident set size) used during the process.

**Implementation Details:**

- The **`benchmark_compression`** function, implemented in **`benchmark/benchmark.c`**, conducts the benchmark tests.
- It uses standard C library functions such as `clock()` for time measurements and system-specific functions like `getrusage` (common in Unix-like systems) to obtain CPU and memory usage information.
- The benchmarking process involves compressing an input file using the specified algorithm and compression level, measuring the time and resources consumed during the operation, decompressing the generated compressed file back to its original form, and again measuring the time taken.
- The results are stored in a `CompressionBenchmark` struct and printed to the console, providing a clear overview of the performance characteristics.

**Usage:**
To use the benchmarking feature, you can run the compressor with the -b flag, specifying the algorithm to be benchmarked and the input file to be used for testing. For example:

```bash
./compressor -b -a rle input.txt
```

This command will benchmark the RLE compression algorithm using input.txt as the test file and output the performance metrics to the console.

**Notes:**

- The benchmarking feature is designed to give a general idea of performance and may vary depending on the system's hardware and software environment.
- Memory usage measurements are particularly system-dependent. The current implementation uses getrusage, which is commonly available on Unix-like systems but might not be directly portable to other environments like Windows.

### Bit Manipulation

Both RLE and, in particular, Huffman coding often require working with data at the bit level. The **`utils/bit_manipulation.h`** and **`utils/bit_manipulation.c`** files provide a set of utility functions for bit-level operations, including:

- **Setting/clearing bits:** `set_bit` and `clear_bit`.
- **Getting bit values:** `get_bit`.
- **Reading/writing bits from/to files:** `read_bit` and `write_bit`
- **Buffer flushing:** `flush_bit_buffer`

These functions help to handle the bit-stream manipulations required for these compression algorithms.

## Error Handling

The **`compressor`** utility has built-in error handling to address various scenarios gracefully. These include:

- **Invalid command-line arguments:** Checks for incorrect usage or missing parameters.
- **Invalid compression mode/algorithm:** Ensures that valid compression modes (-c or -d) and algorithm names (rle or huffman) are provided.
- **File I/O errors:** Handles situations where the input file cannot be opened or read from, or if the output file cannot be created or written to.
- **Unexpected EOF:** Check for premature file end
- **Algorithm-specific errors:** Checks for errors like invalid frequency table during huffman decoding
- **Dynamic Memory allocation error**

In case of an error, the utility prints an informative error message to `stderr` (standard error) and exits with a non-zero status code (typically `1`), which can be used for scripting and automation.

## License

This project is open-source and released under the [MIT License](https://opensource.org/licenses/MIT). See the `LICENSE` file for full details. You are free to use, modify, and distribute the code, subject to the terms of the license.
