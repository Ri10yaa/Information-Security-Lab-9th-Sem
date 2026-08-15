#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "dh.h"

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

    // --- Diffie-Hellman Key Exchange ---
    long long private_a = 7654321; // Client's private key 'a'
    long long public_A = DHCipher::power(DHCipher::GENERATOR_G, private_a, DHCipher::PRIME_P);

    // Send Client's Public Key A to Server
    send(sock, &public_A, sizeof(public_A), 0);
    std::cout << "[Client] Sent Public Key A: " << public_A << "\n";

    // Receive Server's Public Key B
    long long public_B = 0;
    read(sock, &public_B, sizeof(public_B));
    std::cout << "[Client] Received Server Public Key B: " << public_B << "\n";

    // Compute Shared Secret Key K = (B^a) % p
    long long shared_secret = DHCipher::power(public_B, private_a, DHCipher::PRIME_P);
    std::cout << "[Client] Computed Shared Secret Key: " << shared_secret << "\n";

    // --- Encrypt & Send Payload ---
    std::string message = "Secret message payload protected by Diffie-Hellman & AES!";
    std::cout << "[Client] Plaintext: \"" << message << "\"\n";

    try {
        std::string ciphertext = DHCipher::encrypt(message, shared_secret);
        send(sock, ciphertext.data(), ciphertext.length(), 0);
        std::cout << "[Client] Encrypted ciphertext successfully sent to server.\n";
    } catch (const std::exception& e) {
        std::cerr << "[Client] Encryption Error: " << e.what() << "\n";
    }

    close(sock);
    return 0;
}