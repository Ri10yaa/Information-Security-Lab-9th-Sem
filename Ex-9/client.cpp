#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "digital_signature.h"

const int PORT = 8080;

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "[Client] Socket creation error\n";
        return 1;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "[Client] Invalid address\n";
        close(sock);
        return 1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "[Client] Connection failed. Ensure server is running!\n";
        close(sock);
        return 1;
    }

    // 1. Generate RSA Key Pair
    EVP_PKEY* keypair = DigitalSignature::generate_keypair();
    std::cout << "[Client] Generated 2048-bit RSA Key Pair.\n";

    // 2. Export Public Key to PEM format to send to Server
    BIO* bio = BIO_new(BIO_s_mem());
    PEM_write_bio_PUBKEY(bio, keypair);
    char* pub_key_bytes = nullptr;
    long pub_key_len = BIO_get_mem_data(bio, &pub_key_bytes);

    // Send Public Key to Server
    uint32_t net_pub_len = htonl(static_cast<uint32_t>(pub_key_len));
    send(sock, &net_pub_len, sizeof(net_pub_len), 0);
    send(sock, pub_key_bytes, pub_key_len, 0);
    std::cout << "[Client] Sent Public Key to Server.\n";

    // 3. Prepare Message & Sign with Private Key
    std::string message = "Authorized Payment Transfer Request: $5000";
    std::vector<unsigned char> signature = DigitalSignature::sign_message(message, keypair);

    std::cout << "[Client] Message: \"" << message << "\"\n";
    std::cout << "[Client] Created Digital Signature (" << signature.size() << " bytes).\n";

    // 4. Send Message length & Message
    uint32_t net_msg_len = htonl(static_cast<uint32_t>(message.length()));
    send(sock, &net_msg_len, sizeof(net_msg_len), 0);
    send(sock, message.data(), message.length(), 0);

    // 5. Send Signature length & Signature
    uint32_t net_sig_len = htonl(static_cast<uint32_t>(signature.size()));
    send(sock, &net_sig_len, sizeof(net_sig_len), 0);
    send(sock, signature.data(), signature.size(), 0);

    std::cout << "[Client] Sent Message and Digital Signature successfully.\n";

    // Clean up
    BIO_free(bio);
    EVP_PKEY_free(keypair);
    close(sock);
    return 0;
}