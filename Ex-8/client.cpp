#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "sha1_hash.h"

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
        std::cerr << "[Client] Invalid server address\n";
        close(sock);
        return 1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "[Client] Connection failed. Make sure the server is running!\n";
        close(sock);
        return 1;
    }

    std::string message = "The quick brown fox jumps over the lazy dog";
    
    try {
        // Calculate SHA-1 digest
        std::string sha1_digest = SHA1Digest::compute(message);

        std::cout << "[Client] Message:      \"" << message << "\"\n";
        std::cout << "[Client] SHA-1 Digest: " << sha1_digest << "\n";

        // 1. Send Message length and Message
        uint32_t msg_len = htonl(static_cast<uint32_t>(message.length()));
        send(sock, &msg_len, sizeof(msg_len), 0);
        send(sock, message.data(), message.length(), 0);

        // 2. Send SHA-1 Digest
        send(sock, sha1_digest.data(), sha1_digest.length(), 0);

        std::cout << "[Client] Sent message and SHA-1 digest successfully.\n";
    } catch (const std::exception& e) {
        std::cerr << "[Client] Error: " << e.what() << "\n";
    }

    close(sock);
    return 0;
}