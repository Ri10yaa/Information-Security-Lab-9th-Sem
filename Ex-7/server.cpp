#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "dh.h"

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

    // --- Diffie-Hellman Key Exchange ---
    long long private_b = 1234567; // Server's private key 'b'
    long long public_B = DHCipher::power(DHCipher::GENERATOR_G, private_b, DHCipher::PRIME_P);

    // Receive Client's Public Key A
    long long public_A = 0;
    read(new_socket, &public_A, sizeof(public_A));
    std::cout << "[Server] Received Client Public Key A: " << public_A << "\n";

    // Send Server's Public Key B to Client
    send(new_socket, &public_B, sizeof(public_B), 0);
    std::cout << "[Server] Sent Public Key B: " << public_B << "\n";

    // Compute Shared Secret Key K = (A^b) % p
    long long shared_secret = DHCipher::power(public_A, private_b, DHCipher::PRIME_P);
    std::cout << "[Server] Computed Shared Secret Key: " << shared_secret << "\n";

    // --- Receive Encrypted Message ---
    char buffer[1024] = {0};
    ssize_t bytes_read = read(new_socket, buffer, sizeof(buffer));

    if (bytes_read > 0) {
        std::string ciphertext(buffer, bytes_read);
        std::cout << "[Server] Received " << bytes_read << " bytes of encrypted ciphertext.\n";

        try {
            std::string plaintext = DHCipher::decrypt(ciphertext, shared_secret);
            std::cout << "[Server] Decrypted Message: \"" << plaintext << "\"\n";
        } catch (const std::exception& e) {
            std::cerr << "[Server] Decryption Error: " << e.what() << "\n";
        }
    }

    close(new_socket);
    close(server_fd);
    return 0;
}