#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "md5_hash.h"

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
        std::cerr << "[Client] Connection failed. Ensure the server is running!\n";
        close(sock);
        return 1;
    }

    std::string message = "MD5 Digest verification test payload";

    try {
        // Calculate MD5 hash digest
        std::string md5_digest = MD5Digest::compute(message);

        std::cout << "[Client] Message:    \"" << message << "\"\n";
        std::cout << "[Client] MD5 Digest: " << md5_digest << "\n";

        // 1. Send Message length and Message
        uint32_t msg_len = htonl(static_cast<uint32_t>(message.length()));
        send(sock, &msg_len, sizeof(msg_len), 0);
        send(sock, message.data(), message.length(), 0);

        // 2. Send MD5 Digest (32 characters)
        send(sock, md5_digest.data(), md5_digest.length(), 0);

        std::cout << "[Client] Sent message and MD5 digest successfully.\n";
    } catch (const std::exception& e) {
        std::cerr << "[Client] Error: " << e.what() << "\n";
    }

    close(sock);
    return 0;
}