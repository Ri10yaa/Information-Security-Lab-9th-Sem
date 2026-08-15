#include <iostream>
#include <cstring>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "md5_hash.h"

const int PORT = 8080;

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        std::cerr << "[Server] Socket creation failed\n";
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

    if (listen(server_fd, 1) < 0) {
        std::cerr << "[Server] Listen failed\n";
        close(server_fd);
        return 1;
    }
    std::cout << "[Server] Listening on port " << PORT << "...\n";

    int addrlen = sizeof(address);
    int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        std::cerr << "[Server] Accept failed\n";
        close(server_fd);
        return 1;
    }

    // 1. Read message length and message
    uint32_t msg_len = 0;
    read(new_socket, &msg_len, sizeof(msg_len));
    msg_len = ntohl(msg_len);

    std::vector<char> msg_buf(msg_len + 1, 0);
    read(new_socket, msg_buf.data(), msg_len);
    std::string received_msg(msg_buf.data(), msg_len);

    // 2. Read client's MD5 digest (32 characters)
    char client_digest_buf[33] = {0};
    read(new_socket, client_digest_buf, 32);
    std::string client_digest(client_digest_buf);

    std::cout << "\n[Server] Received Plaintext Message: \"" << received_msg << "\"\n";
    std::cout << "[Server] Received MD5 Digest:       " << client_digest << "\n";

    try {
        // Recalculate MD5 digest locally on server
        std::string server_digest = MD5Digest::compute(received_msg);
        std::cout << "[Server] Computed MD5 Digest:       " << server_digest << "\n";

        // Integrity verification
        if (server_digest == client_digest) {
            std::cout << "[Server] ✅ INTEGRITY VERIFIED: MD5 Digests match perfectly!\n";
        } else {
            std::cout << "[Server] ❌ INTEGRITY FAILED: Message has been altered!\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "[Server] Error: " << e.what() << "\n";
    }

    close(new_socket);
    close(server_fd);
    return 0;
}