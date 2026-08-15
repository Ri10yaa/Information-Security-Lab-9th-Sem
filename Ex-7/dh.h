#ifndef DH_CIPHER_H
#define DH_CIPHER_H

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <stdexcept>
#include <openssl/evp.h>
#include <openssl/sha.h>

// Renamed namespace to DHCipher to avoid collision with OpenSSL's typedef 'DH'
namespace DHCipher {

// Standard Prime (p) and Generator (g) for DH exchange
constexpr long long PRIME_P = 1000000007LL; // Large prime
constexpr long long GENERATOR_G = 5LL;      // Primitive root modulo P

/**
 * Computes (base^exp) % mod using binary modular exponentiation.
 * Uses 128-bit integers internally to prevent arithmetic overflow.
 */
inline long long power(long long base, long long exp, long long mod) {
    long long res = 1;
    base = base % mod;
    while (exp > 0) {
        if (exp % 2 == 1) {
            res = static_cast<long long>((static_cast<__int128>(res) * base) % mod);
        }
        base = static_cast<long long>((static_cast<__int128>(base) * base) % mod);
        exp /= 2;
    }
    return res;
}

/**
 * Derives a 256-bit AES key and 128-bit IV from the numeric DH shared secret.
 */
inline void derive_key_and_iv(long long shared_secret, unsigned char* key, unsigned char* iv) {
    std::string secret_str = std::to_string(shared_secret);
    
    // Hash shared secret string with SHA-256 to create a deterministic 32-byte key
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(secret_str.data()), secret_str.length(), hash);

    std::memcpy(key, hash, 32);                              // 256-bit key
    std::memcpy(iv, hash + (SHA256_DIGEST_LENGTH - 16), 16); // 128-bit IV
}

/**
 * Encrypts plaintext using AES-256-CBC with the derived DH key.
 */
inline std::string encrypt(const std::string& plaintext, long long shared_secret) {
    unsigned char key[32], iv[16];
    derive_key_and_iv(shared_secret, key, iv);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create EVP context.");

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("AES EncryptInit failed.");
    }

    std::vector<unsigned char> ciphertext(plaintext.length() + EVP_MAX_BLOCK_LENGTH);
    int len = 0, total_len = 0;

    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len,
                          reinterpret_cast<const unsigned char*>(plaintext.data()),
                          static_cast<int>(plaintext.length())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("AES EncryptUpdate failed.");
    }
    total_len += len;

    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("AES EncryptFinal failed.");
    }
    total_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return std::string(reinterpret_cast<char*>(ciphertext.data()), total_len);
}

/**
 * Decrypts AES-256-CBC ciphertext back into plaintext using the DH key.
 */
inline std::string decrypt(const std::string& ciphertext, long long shared_secret) {
    unsigned char key[32], iv[16];
    derive_key_and_iv(shared_secret, key, iv);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create EVP context.");

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("AES DecryptInit failed.");
    }

    std::vector<unsigned char> plaintext(ciphertext.length() + EVP_MAX_BLOCK_LENGTH);
    int len = 0, total_len = 0;

    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len,
                          reinterpret_cast<const unsigned char*>(ciphertext.data()),
                          static_cast<int>(ciphertext.length())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("AES DecryptUpdate failed.");
    }
    total_len += len;

    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("AES DecryptFinal failed - incorrect key or corrupted payload.");
    }
    total_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return std::string(reinterpret_cast<char*>(plaintext.data()), total_len);
}

} // namespace DHCipher

#endif // DH_CIPHER_H