#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "des.h"

const int PORT = 8080;
const unsigned char SHARED_KEY[8] = {'M', 'y', 'S', 'e', 'c', 'r', 'e', 't'};

int main() {
    // 1. Create socket file descriptor
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        std::cerr << "[Server] Socket creation failed\n";
        return 1;
    }

    // 2. Allow port reuse
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 3. Bind socket to IP/Port
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "[Server] Bind failed\n";
        close(server_fd);
        return 1;
    }

    // 4. Start listening
    if (listen(server_fd, 1) < 0) {
        std::cerr << "[Server] Listen failed\n";
        close(server_fd);
        return 1;
    }
    std::cout << "[Server] Waiting for incoming client on port " << PORT << "...\n";

    // 5. Accept connection
    int addrlen = sizeof(address);
    int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        std::cerr << "[Server] Connection accept failed\n";
        close(server_fd);
        return 1;
    }
    std::cout << "[Server] Client connected!\n";

    // 6. Read encrypted payload from network socket
    char buffer[1024] = {0};
    ssize_t bytes_read = read(new_socket, buffer, sizeof(buffer));

    if (bytes_read > 0) {
        std::string ciphertext(buffer, bytes_read);
        std::cout << "[Server] Received " << bytes_read << " encrypted bytes.\n";

        try {
            // Decrypt using des_cipher.h helper function
            std::string plaintext = DES::decrypt(ciphertext, SHARED_KEY);
            std::cout << "[Server] Decrypted Message: \"" << plaintext << "\"\n";
        } catch (const std::exception& e) {
            std::cerr << "[Server] Decryption Error: " << e.what() << "\n";
        }
    }

    // 7. Clean up
    close(new_socket);
    close(server_fd);
    return 0;
}