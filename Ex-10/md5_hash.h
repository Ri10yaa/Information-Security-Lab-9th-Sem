#ifndef MD5_HASH_H
#define MD5_HASH_H

#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <openssl/evp.h>
#include <openssl/provider.h> // Required for OpenSSL 3.x legacy algorithms like MD5

namespace MD5Digest {

// Helper to load legacy provider required for MD5 in OpenSSL 3.x+
inline void load_providers() {
    static bool loaded = false;
    if (!loaded) {
        OSSL_PROVIDER_load(nullptr, "legacy");
        OSSL_PROVIDER_load(nullptr, "default");
        loaded = true;
    }
}

/**
 * Computes the MD5 digest of an input string.
 * 
 * @param input Plaintext string to hash.
 * @return 32-character Hexadecimal MD5 hash digest.
 */
inline std::string compute(const std::string& input) {
    load_providers();

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create EVP_MD_CTX");
    }

    // Initialize context for MD5
    if (EVP_DigestInit_ex(ctx, EVP_md5(), nullptr) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize MD5 digest context");
    }

    // Hash input data
    if (EVP_DigestUpdate(ctx, input.data(), input.length()) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to update MD5 digest");
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len = 0;

    // Finalize digest computation
    if (EVP_DigestFinal_ex(ctx, hash, &hash_len) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize MD5 digest");
    }

    EVP_MD_CTX_free(ctx);

    // Convert raw byte array to formatted hex string
    std::ostringstream hex_stream;
    for (unsigned int i = 0; i < hash_len; ++i) {
        hex_stream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return hex_stream.str();
}

} // namespace MD5Digest

#endif // MD5_HASH_H