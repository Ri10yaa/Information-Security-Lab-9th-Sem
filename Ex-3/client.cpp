#include "railfence.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);

    cout << "Receiver is listening on port 8080..." << endl;

    int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    
    char buffer[1024] = {0};
    read(new_socket, buffer, 1024);

    string payload(buffer);
    size_t delimiter = payload.find(':');
    if (delimiter == string::npos) {
        cerr << "Error: Invalid packet payload data format." << endl;
        return -1;
    }

    string ciphertext= payload.substr(0, delimiter);
    int rails = stoi(payload.substr(delimiter + 1));

    cout << "\n--- Transmission Received ---" << endl;
    cout << "Number of rails   : " << rails << endl;
    
    cout << "Ciphertext     : " << ciphertext << endl;

    string decryptedText = decrypt(ciphertext,rails);

    cout << "Decrypted Text : " << decryptedText << endl;
    cout << "-----------------------------" << endl;

    close(new_socket);
    close(server_fd);
    return 0;
}