#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <map>
#include <mutex>

class HttpServer 
{
public:
    HttpServer(int port);
    void start();

private:
    int server_fd;
    sockaddr_in address;
    int port;

    void handleClient(int client_socket);
    std::string generateFileResponse(const std::string& filePath);
    std::string extractPostBody(const std::string& request);
    std::string extractParameter(const std::string& bode, const std::string& param);
    std::string extractFilePath(const std::string& request);
    std::string generateHttpResponse(const std::string& request);
    void logRequest(const std::string& logMessage);
    std::string generateWelcomeResponse();
    std::string generateLoginPageResponse();
};

#endif