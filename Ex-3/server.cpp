#include<arpa/inet.h>
#include<sys/socket.h>
#include<iostream>
#include<unistd.h>
#include<cstring>
#include "railfence.h"

using namespace std;

int main(){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "Socket creation failed." << endl;
        return -1;
    }

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "Connection to receiver failed." << endl;
        return -1;
    }

    string plaintext = "garden";
    int rails = 2;
    string ct = encrypt(plaintext,rails);
    string payload = ct + ":" + to_string(rails);
    
    send(sock, payload.c_str(), payload.length(), 0);
    cout << "Payload sent successfully!" << endl;

    close(sock);
    return 0;
}