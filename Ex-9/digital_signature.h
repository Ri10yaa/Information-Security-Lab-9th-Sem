#ifndef DIGITAL_SIGNATURE_H
#define DIGITAL_SIGNATURE_H

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

namespace DigitalSignature {

/**
 * Generates a 2048-bit RSA key pair.
 */
inline EVP_PKEY* generate_keypair() {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (!ctx) throw std::runtime_error("Failed to create PKEY context.");

    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Keygen init failed.");
    }

    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Failed to set RSA key length.");
    }

    EVP_PKEY* pkey = nullptr;
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Key generation failed.");
    }

    EVP_PKEY_CTX_free(ctx);
    return pkey;
}

/**
 * Signs a message using SHA-256 and an RSA Private Key.
 */
inline std::vector<unsigned char> sign_message(const std::string& message, EVP_PKEY* private_key) {
    EVP_MD_CTX* md_ctx = EVP_MD_CTX_new();
    if (!md_ctx) throw std::runtime_error("Failed to create MD context.");

    if (EVP_DigestSignInit(md_ctx, nullptr, EVP_sha256(), nullptr, private_key) <= 0) {
        EVP_MD_CTX_free(md_ctx);
        throw std::runtime_error("DigestSignInit failed.");
    }

    if (EVP_DigestSignUpdate(md_ctx, message.data(), message.length()) <= 0) {
        EVP_MD_CTX_free(md_ctx);
        throw std::runtime_error("DigestSignUpdate failed.");
    }

    size_t sig_len = 0;
    // Determine signature buffer size
    if (EVP_DigestSignFinal(md_ctx, nullptr, &sig_len) <= 0) {
        EVP_MD_CTX_free(md_ctx);
        throw std::runtime_error("DigestSignFinal failed to get length.");
    }

    std::vector<unsigned char> signature(sig_len);
    if (EVP_DigestSignFinal(md_ctx, signature.data(), &sig_len) <= 0) {
        EVP_MD_CTX_free(md_ctx);
        throw std::runtime_error("DigestSignFinal failed.");
    }

    EVP_MD_CTX_free(md_ctx);
    return signature;
}

/**
 * Verifies a signature against a message using SHA-256 and an RSA Public Key.
 */
inline bool verify_signature(const std::string& message, 
                            const std::vector<unsigned char>& signature, 
                            EVP_PKEY* public_key) {
    EVP_MD_CTX* md_ctx = EVP_MD_CTX_new();
    if (!md_ctx) throw std::runtime_error("Failed to create MD context.");

    if (EVP_DigestVerifyInit(md_ctx, nullptr, EVP_sha256(), nullptr, public_key) <= 0) {
        EVP_MD_CTX_free(md_ctx);
        throw std::runtime_error("DigestVerifyInit failed.");
    }

    if (EVP_DigestVerifyUpdate(md_ctx, message.data(), message.length()) <= 0) {
        EVP_MD_CTX_free(md_ctx);
        throw std::runtime_error("DigestVerifyUpdate failed.");
    }

    int result = EVP_DigestVerifyFinal(md_ctx, signature.data(), signature.size());
    EVP_MD_CTX_free(md_ctx);

    return (result == 1); // 1 = Valid, 0 = Invalid
}

} // namespace DigitalSignature

#endif // DIGITAL_SIGNATURE_H