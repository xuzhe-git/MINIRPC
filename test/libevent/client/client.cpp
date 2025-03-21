#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888
#define BUFFER_SIZE 1024

int main() {

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }


    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address or address not supported" << std::endl;
        close(sockfd);
        return 1;
    }


    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Failed to connect to server" << std::endl;
        close(sockfd);
        return 1;
    }

    std::cout << "Connected to server: " << SERVER_IP << ":" << SERVER_PORT << std::endl;


    const char* message = "Hello, server!";
    ssize_t sentBytes = send(sockfd, message, strlen(message), 0);
    if (sentBytes == -1) {
        std::cerr << "Failed to send message to server" << std::endl;
        close(sockfd);
        return 1;
    }
    std::cout << "Sent message: " << message << std::endl;


    char buffer[BUFFER_SIZE];
    ssize_t recvBytes = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
    if (recvBytes == -1) {
        std::cerr << "Failed to receive message from server" << std::endl;
        close(sockfd);
        return 1;
    }
    buffer[recvBytes] = '\0';
    std::cout << "Received message from server: " << buffer << std::endl;


    close(sockfd);

    return 0;
}