#ifndef SHA1_HASH_H
#define SHA1_HASH_H

#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <openssl/evp.h>

// Using SHA1Digest namespace to prevent identifier collisions with OpenSSL types
namespace SHA1Digest {

/**
 * Computes the SHA-1 hash of an input string and returns it as a 40-character hexadecimal string.
 * 
 * @param input The text to hash.
 * @return Hexadecimal representation of the SHA-1 digest.
 */
inline std::string compute(const std::string& input) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create EVP_MD_CTX");
    }

    // Initialize context for SHA-1
    if (EVP_DigestInit_ex(ctx, EVP_sha1(), nullptr) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize SHA-1 digest context");
    }

    // Hash input data
    if (EVP_DigestUpdate(ctx, input.data(), input.length()) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to update SHA-1 digest");
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len = 0;

    // Finalize digest computation
    if (EVP_DigestFinal_ex(ctx, hash, &hash_len) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize SHA-1 digest");
    }

    EVP_MD_CTX_free(ctx);

    // Convert raw byte array to formatted hex string
    std::ostringstream hex_stream;
    for (unsigned int i = 0; i < hash_len; ++i) {
        hex_stream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return hex_stream.str();
}

} // namespace SHA1Digest

#endif // SHA1_HASH_H