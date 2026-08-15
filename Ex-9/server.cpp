#include <iostream>
#include <cstring>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "digital_signature.h"

const int PORT = 8080;

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        std::cerr << "[Server] Socket error\n";
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "[Server] Bind failed\n";
        close(server_fd);
        return 1;
    }

    listen(server_fd, 1);
    std::cout << "[Server] Listening on port " << PORT << "...\n";

    int addrlen = sizeof(address);
    int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        std::cerr << "[Server] Accept failed\n";
        close(server_fd);
        return 1;
    }

    // 1. Receive Client's Public Key
    uint32_t net_pub_len = 0;
    read(new_socket, &net_pub_len, sizeof(net_pub_len));
    uint32_t pub_len = ntohl(net_pub_len);

    std::vector<char> pub_buf(pub_len);
    read(new_socket, pub_buf.data(), pub_len);

    BIO* bio = BIO_new_mem_buf(pub_buf.data(), pub_len);
    EVP_PKEY* client_public_key = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);

    if (!client_public_key) {
        std::cerr << "[Server] Failed to parse client's public key.\n";
        close(new_socket);
        close(server_fd);
        return 1;
    }
    std::cout << "[Server] Successfully received and parsed Client's Public Key.\n";

    // 2. Receive Message
    uint32_t net_msg_len = 0;
    read(new_socket, &net_msg_len, sizeof(net_msg_len));
    uint32_t msg_len = ntohl(net_msg_len);

    std::vector<char> msg_buf(msg_len);
    read(new_socket, msg_buf.data(), msg_len);
    std::string message(msg_buf.data(), msg_len);

    // 3. Receive Digital Signature
    uint32_t net_sig_len = 0;
    read(new_socket, &net_sig_len, sizeof(net_sig_len));
    uint32_t sig_len = ntohl(net_sig_len);

    std::vector<unsigned char> signature(sig_len);
    read(new_socket, signature.data(), sig_len);

    std::cout << "\n[Server] Received Message: \"" << message << "\"\n";
    std::cout << "[Server] Received Digital Signature Size: " << sig_len << " bytes.\n";

    // 4. Verify Digital Signature
    try {
        bool is_valid = DigitalSignature::verify_signature(message, signature, client_public_key);
        if (is_valid) {
            std::cout << "[Server] ✅ SIGNATURE VALID: The message is authentic and untampered!\n";
        } else {
            std::cout << "[Server] ❌ SIGNATURE INVALID: Verification failed!\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "[Server] Verification Error: " << e.what() << "\n";
    }

    EVP_PKEY_free(client_public_key);
    close(new_socket);
    close(server_fd);
    return 0;
}