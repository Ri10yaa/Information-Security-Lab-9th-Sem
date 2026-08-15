#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "des.h"

const int PORT = 8080;
const unsigned char SHARED_KEY[8] = {'M', 'y', 'S', 'e', 'c', 'r', 'e', 't'};

int main() {
    // 1. Create socket file descriptor
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "[Client] Socket creation error\n";
        return 1;
    }

    // 2. Configure target address
    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "[Client] Invalid server IP address\n";
        close(sock);
        return 1;
    }

    // 3. Connect to the server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "[Client] Connection failed. Ensure server is running!\n";
        close(sock);
        return 1;
    }

    std::string message = "Top Secret Payload using des.h!";
    std::cout << "[Client] Plaintext: \"" << message << "\"\n";

    try {
        // Encrypt using des_cipher.h helper function
        std::string ciphertext = DES::encrypt(message, SHARED_KEY);

        // Send ciphertext over socket
        send(sock, ciphertext.data(), ciphertext.length(), 0);
        std::cout << "[Client] Encrypted ciphertext successfully sent to server.\n";
    } catch (const std::exception& e) {
        std::cerr << "[Client] Encryption Error: " << e.what() << "\n";
    }

    // 4. Clean up
    close(sock);
    return 0;
}