#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <stdio.h>

/**
 * @brief Encrypts the compressed file with a password.
 * 
 * @param compressed_file File pointer to the compressed data.
 * @param output_file File pointer to store the encrypted data.
 * @param password Encryption password.
 * @return int 0 on success, -1 on error.
 */
int encrypt_compressed_file(FILE *compressed_file, FILE *output_file, const char *password);

/**
 * @brief Decrypts the encrypted file with a password.
 * 
 * @param encrypted_file File pointer to the encrypted data.
 * @param output_file File pointer to store the decrypted data.
 * @param password Decryption password.
 * @return int 0 on success, -1 on error.
 */
int decrypt_compressed_file(FILE *encrypted_file, FILE *output_file, const char *password);

#endif // ENCRYPTION_H