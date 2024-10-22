#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() 
{
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    char buffer[BUFFER_SIZE];

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) 
    {
        std::cerr << "Socket creation failed." << std::endl;
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; 
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) 
    {
        std::cerr << "Bind failed." << std::endl;
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 3) < 0) 
    {
        std::cerr << "Listen failed." << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Server listening on http://127.0.0.1:" << PORT << std::endl;

    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
    if (clientSocket < 0) 
    {
        std::cerr << "Accept failed." << std::endl;
        close(serverSocket);
        return 1;
    }

    while (true) 
    {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived < 0) 
        {
            std::cerr << "Receive failed." << std::endl;
            break;
        }
        if (bytesReceived == 0) 
        {
            std::cout << "Client disconnected." << std::endl;
            break;
        }

        std::cout << "Received: " << buffer << std::endl;
        send(clientSocket, buffer, bytesReceived, 0);  
    }

    close(clientSocket);
    close(serverSocket);
    return 0;
}
