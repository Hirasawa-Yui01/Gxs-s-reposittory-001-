#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() 
{
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) 
    {
        std::cerr << "Socket creation failed." << std::endl;
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(PORT);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) 
    {
        std::cerr << "Connection failed." << std::endl;
        close(clientSocket);
        return 1;
    }

    while (true) 
    {
        std::cout << "Enter message (or 'exit' to quit): ";
        std::cin.getline(buffer, BUFFER_SIZE);
        if (strcmp(buffer, "exit") == 0) 
        {
            break;
        }

        if (send(clientSocket, buffer, strlen(buffer), 0) < 0) 
        {
            std::cerr << "Send failed." << std::endl;
            break;
        }

        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived > 0) 
        {
            std::cout << "Echo from server: " << buffer << std::endl;
        } 
        else 
        {
            std::cerr << "Receive failed or connection closed." << std::endl;
            break;
        }
    }

    close(clientSocket);
    return 0;
}

