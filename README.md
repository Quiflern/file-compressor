# File Compressor

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## Description

This project is a command-line file compression and decompression utility implemented in C. It utilizes two lossless data compression algorithms:

1. **Run-Length Encoding (RLE):** A straightforward algorithm that replaces consecutive repeating characters with a count and the character.
2. **Huffman Coding:** A more advanced algorithm that assigns variable-length codes to characters based on their frequency of occurrence, resulting in shorter codes for frequently used characters.

The utility allows you to select either algorithm for both compression and decompression, offering flexibility and the potential for improved compression ratios, especially with Huffman coding for suitable file types.

## Repository Structure

```
file-compressor/
├── huffman/              # Huffman coding implementation
│   ├── huffman.h         # Header file for Huffman functions
│   ├── huffman_compress.c # Huffman compression algorithm
│   └── huffman_decompress.c # Huffman decompression algorithm
├── rle/                  # Run-Length Encoding implementation
│   ├── rle.h             # Header file for RLE functions
│   ├── rle_compress.c    # RLE compression algorithm
│   └── rle_decompress.c  # RLE decompression algorithm
├── sample-files/         # Sample files for testing
└── └── sample.txt
├── utils/                # Utility functions
│   ├── bit_manipulation.c# Bit-level operation functions
│   └── bit_manipulation.h# Header for bit manipulation
├── Makefile              # Makefile for project compilation
└── main.c                # Main program & command-line interface
```
## Getting Started

These instructions will guide you through setting up the project, building the executable, and running the file compressor on your local machine.

### Prerequisites

Before you begin, ensure you have the following software installed on your system:

*   **C Compiler:** A standard C compiler, such as GCC (GNU Compiler Collection).
*   **Make:** The `make` utility to automate the compilation process using the provided Makefile.

You can typically install these tools through your operating system's package manager. For example:

*   **Debian/Ubuntu:** `sudo apt-get install build-essential`
*   **macOS:** Xcode Command Line Tools (which include `make` and `gcc`)
*   **Fedora/CentOS/RHEL:** `sudo dnf groupinstall "Development Tools"` or `sudo yum groupinstall "Development Tools"`
*   **Windows:** You can use MinGW or Cygwin to get a GCC and Make environment on Windows.

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
./compressor [-c|-d] [-a rle|huffman] input_file output_file
```

#### Arguments

*   **`-c`:**  Indicates compression mode.
*   **`-d`:**  Indicates decompression mode.
*   **`-a [rle|huffman]`:** Specifies the compression algorithm:
    *   `rle`: Use Run-Length Encoding.
    *   `huffman`: Use Huffman Coding.
    *   **Default:** `rle` is used if the `-a` flag is omitted.
*   `input_file`: The path to the file you want to compress or decompress.
*   `output_file`: The desired path for the output file (compressed or decompressed).

#### Examples

1. **Compress using RLE:**

    ```bash
    ./compressor -c -a rle sample-files/sample.txt compressed.rle
    ```

2. **Decompress using RLE:**

    ```bash
    ./compressor -d -a rle compressed.rle decompressed.txt
    ```

3. **Compress using Huffman Coding:**

    ```bash
    ./compressor -c -a huffman sample-files/sample.txt compressed.huff
    ```

4. **Decompress using Huffman Coding:**

    ```bash
    ./compressor -d -a huffman compressed.huff decompressed.txt
    ```

## Algorithm Details
### Run-Length Encoding (RLE)

Run-Length Encoding is most effective for data with long sequences of repeating characters (e.g., images with solid color areas, text with many repeated spaces). It works by:

1. Reading consecutive characters from the input file.
2. Counting the number of times a character repeats consecutively.
3. Storing the count and the character in the compressed file.

**Implementation Files:**

*   `rle/rle.h`: Declares the RLE compression and decompression functions (`rle_compress` and `rle_decompress`).
*   `rle/rle_compress.c`: Contains the implementation of RLE compression logic.
*   `rle/rle_decompress.c`: Contains the implementation of RLE decompression logic.


### Huffman Coding

Huffman coding is generally more effective than RLE for a wider range of data, as it takes advantage of character frequencies. Here's how it works:

1. **Frequency Analysis:** It calculates the frequency of each character in the input file.
2. **Huffman Tree Construction:** Builds a binary tree where:
    *   Leaves represent characters.
    *   Paths from the root to leaves represent codes (shorter paths for frequent characters).
    *   The tree is constructed in a way that ensures unique decodability.
3. **Code Assignment:** Assigns variable-length binary codes to characters based on the tree.
4. **Encoding:** Replaces each character in the input file with its assigned Huffman code.

**Implementation Files:**

*   `huffman/huffman.h`: Declares Huffman coding-related functions and data structures (e.g., `HuffmanNode`, `huffman_compress`, `huffman_decompress`).
*   `huffman/huffman_compress.c`: Implements Huffman compression, including frequency analysis, Huffman tree construction, code generation, and encoding.
*   `huffman/huffman_decompress.c`: Implements Huffman decompression, including reading the frequency table, rebuilding the Huffman tree, and decoding.

### Bit Manipulation

Both RLE and, in particular, Huffman coding often require working with data at the bit level. The `utils/bit_manipulation.h` and `utils/bit_manipulation.c` files provide a set of utility functions for bit-level operations, including:

*   **Setting/clearing bits:** `set_bit` and `clear_bit`.
*   **Getting bit values:** `get_bit`.
*   **Reading/writing bits from/to files:** `read_bit` and `write_bit`
*   **Buffer flushing** `flush_bit_buffer`

These functions help to handle the bit-stream manipulations required for these compression algorithms.

## Error Handling

The `compressor` utility has built-in error handling to address various scenarios gracefully. These include:

*   **Invalid command-line arguments:** Checks for incorrect usage or missing parameters.
*   **Invalid compression mode/algorithm:** Ensures that valid compression modes (-c or -d) and algorithm names (rle or huffman) are provided.
*   **File I/O errors:** Handles situations where the input file cannot be opened or read from, or if the output file cannot be created or written to.
*   **Unexpected EOF:** Check for premature file end
*   **Algorithm-specific errors:** Checks for errors like invalid frequency table during huffman decoding
*   **Dynamic Memory allocation error**

In case of an error, the utility prints an informative error message to `stderr` (standard error) and exits with a non-zero status code (typically `1`), which can be used for scripting and automation.


## License

This project is open-source and released under the [MIT License](https://opensource.org/licenses/MIT). See the `LICENSE` file for full details. You are free to use, modify, and distribute the code, subject to the terms of the license.