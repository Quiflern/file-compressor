
#include "encryption.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string.h>

// Salt length
#define SALT_LENGTH 8

// Key length for AES-256
#define KEY_LENGTH 32

// IV length for AES
#define IV_LENGTH 16

// PBKDF2 iterations
#define PBKDF2_ITERATIONS 10000

int encrypt_compressed_file(FILE *compressed_file, FILE *output_file, const char *password) {
    unsigned char key[KEY_LENGTH];
    unsigned char iv[IV_LENGTH];
    unsigned char salt[SALT_LENGTH];

    // Generate a random salt
    if (RAND_bytes(salt, SALT_LENGTH) != 1) {
        fprintf(stderr, "Error generating salt\n");
        return -1;
    }

    // Derive key from password using PBKDF2
    if (PKCS5_PBKDF2_HMAC(password, strlen(password), salt, SALT_LENGTH, PBKDF2_ITERATIONS, EVP_sha256(), KEY_LENGTH, key) != 1) {
        fprintf(stderr, "Error deriving key\n");
        return -1;
    }

    // Generate a random IV
    if (RAND_bytes(iv, IV_LENGTH) != 1) {
        fprintf(stderr, "Error generating IV\n");
        return -1;
    }

    // Write the salt and IV to the output file
    if (fwrite(salt, 1, SALT_LENGTH, output_file) != SALT_LENGTH) {
        perror("Error writing salt");
        return -1;
    }
    if (fwrite(iv, 1, IV_LENGTH, output_file) != IV_LENGTH) {
        perror("Error writing IV");
        return -1;
    }

    // Initialize encryption context
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        fprintf(stderr, "Error creating cipher context\n");
        return -1;
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) {
        fprintf(stderr, "Error initializing encryption\n");
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    // Encrypt the compressed file
    int bytes_read;
    unsigned char in_buf[4096], out_buf[4096 + EVP_MAX_BLOCK_LENGTH];
    int out_len;
    
    rewind(compressed_file);

    while ((bytes_read = fread(in_buf, 1, sizeof(in_buf), compressed_file)) > 0) {
        if (EVP_EncryptUpdate(ctx, out_buf, &out_len, in_buf, bytes_read) != 1) {
            fprintf(stderr, "Error encrypting data\n");
            EVP_CIPHER_CTX_free(ctx);
            return -1;
        }

        if (fwrite(out_buf, 1, out_len, output_file) != out_len) {
            perror("Error writing encrypted data");
            EVP_CIPHER_CTX_free(ctx);
            return -1;
        }
    }

    if (EVP_EncryptFinal_ex(ctx, out_buf, &out_len) != 1) {
        fprintf(stderr, "Error finalizing encryption\n");
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    if (fwrite(out_buf, 1, out_len, output_file) != out_len) {
        perror("Error writing final encrypted data");
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    // Clean up
    EVP_CIPHER_CTX_free(ctx);
    return 0;
}

int decrypt_compressed_file(FILE *encrypted_file, FILE *output_file, const char *password) {
    unsigned char key[KEY_LENGTH];
    unsigned char iv[IV_LENGTH];
    unsigned char salt[SALT_LENGTH];

    // Read the salt and IV from the input file
    if (fread(salt, 1, SALT_LENGTH, encrypted_file) != SALT_LENGTH) {
        perror("Error reading salt");
        return -1;
    }
    if (fread(iv, 1, IV_LENGTH, encrypted_file) != IV_LENGTH) {
        perror("Error reading IV");
        return -1;
    }

    // Derive key from password using PBKDF2
    if (PKCS5_PBKDF2_HMAC(password, strlen(password), salt, SALT_LENGTH, PBKDF2_ITERATIONS, EVP_sha256(), KEY_LENGTH, key) != 1) {
        fprintf(stderr, "Error deriving key\n");
        return -1;
    }

    // Initialize decryption context
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        fprintf(stderr, "Error creating cipher context\n");
        return -1;
    }

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) {
        fprintf(stderr, "Error initializing decryption\n");
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    // Decrypt the encrypted file
    int bytes_read;
    unsigned char in_buf[4096], out_buf[4096 + EVP_MAX_BLOCK_LENGTH];
    int out_len;

    while ((bytes_read = fread(in_buf, 1, sizeof(in_buf), encrypted_file)) > 0) {
        if (EVP_DecryptUpdate(ctx, out_buf, &out_len, in_buf, bytes_read) != 1) {
            fprintf(stderr, "Error decrypting data\n");
            EVP_CIPHER_CTX_free(ctx);
            return -1;
        }

        if (fwrite(out_buf, 1, out_len, output_file) != out_len) {
            perror("Error writing decrypted data");
            EVP_CIPHER_CTX_free(ctx);
            return -1;
        }
    }

    if (EVP_DecryptFinal_ex(ctx, out_buf, &out_len) != 1) {
        fprintf(stderr, "Error finalizing decryption\n");
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    if (fwrite(out_buf, 1, out_len, output_file) != out_len) {
        perror("Error writing final decrypted data");
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    // Clean up
    EVP_CIPHER_CTX_free(ctx);
    return 0;
}