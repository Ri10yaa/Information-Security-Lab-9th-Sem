#ifndef DES_CIPHER_H
#define DES_CIPHER_H

#include <string>
#include <vector>
#include <stdexcept>
#include <openssl/evp.h>
#include <openssl/provider.h> 

namespace DES {

constexpr size_t KEY_SIZE = 8;

inline void load_providers() {
    static bool loaded = false;
    if (!loaded) {
        OSSL_PROVIDER_load(nullptr, "legacy");
        OSSL_PROVIDER_load(nullptr, "default");
        loaded = true;
    }
}

inline std::string encrypt(const std::string& plaintext, const unsigned char* key) {
    if (!key) {
        throw std::invalid_argument("DES key cannot be null.");
    }

    load_providers();

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create EVP Cipher Context.");
    }

    if (EVP_EncryptInit_ex(ctx, EVP_des_ecb(), nullptr, key, nullptr) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize DES Encryption.");
    }

    std::vector<unsigned char> ciphertext(plaintext.length() + EVP_MAX_BLOCK_LENGTH);
    int len = 0;
    int total_len = 0;

    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, 
                          reinterpret_cast<const unsigned char*>(plaintext.data()), 
                          static_cast<int>(plaintext.length())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EVP_EncryptUpdate failed.");
    }
    total_len += len;

    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EVP_EncryptFinal_ex failed.");
    }
    total_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return std::string(reinterpret_cast<char*>(ciphertext.data()), total_len);
}

inline std::string decrypt(const std::string& ciphertext, const unsigned char* key) {
    if (!key) {
        throw std::invalid_argument("DES key cannot be null.");
    }

    load_providers();

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create EVP Cipher Context.");
    }

    if (EVP_DecryptInit_ex(ctx, EVP_des_ecb(), nullptr, key, nullptr) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize DES Decryption.");
    }

    std::vector<unsigned char> plaintext(ciphertext.length() + EVP_MAX_BLOCK_LENGTH);
    int len = 0;
    int total_len = 0;

    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, 
                          reinterpret_cast<const unsigned char*>(ciphertext.data()), 
                          static_cast<int>(ciphertext.length())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EVP_DecryptUpdate failed.");
    }
    total_len += len;

    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EVP_DecryptFinal_ex failed — invalid padding or wrong key.");
    }
    total_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return std::string(reinterpret_cast<char*>(plaintext.data()), total_len);
}

} 

#endif 