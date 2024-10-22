#include "HttpServer.h"
#include "ThreadPool.h"
#include "Database.h"
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctime>

HttpServer::HttpServer(int port) : port(port) 
{
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(port);
    bind(server_fd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);
}

void HttpServer::logRequest(const std::string& logMessage)
{
    std::ofstream logFile("server.log", std::ios::app);
    if (logFile.is_open())
    {
        std::time_t now = std::time(nullptr);
        std::tm *tm_now = std::localtime(&now);
        char timeBuffer[100];
        std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", tm_now);

        logFile << "[" << timeBuffer << "] " << logMessage << std::endl;
        logFile.close();
    }
    else
    {
        std::cerr << "Unable to open log file." << std::endl;
    }
}

void HttpServer::handleClient(int client_socket) 
{
    char buffer[1024] = {0};
    ssize_t bytesRead = read(client_socket, buffer, 1024);
    if (bytesRead <= 0) 
    {
        close(client_socket);
        return;
    }

    std::string request(buffer, bytesRead);
    std::string response;

    std::string logMessage = "Received request: " + request;
    logRequest(logMessage);

    if (request.find("POST") != std::string::npos && request.find("/login") != std::string::npos) 
    {
        std::string postBody = extractPostBody(request);
        std::string username = extractParameter(postBody, "username");
        std::string password = extractParameter(postBody, "password");

        Database db("users.db");
        bool loginSuccess = db.verifyUserByNameAndPassword(username, password);
        
        if (loginSuccess) 
        {
            response = "HTTP/1.1 302 Found\nLocation: /welcome\n\n";
        } else 
        {
            response = "HTTP/1.1 401 Unauthorized\nContent-Length: 43\n\n<html><body><h1>Login Failed!</h1></body></html>";
        }
    } 
    else if (request.find("GET") != std::string::npos) 
    {
        std::string filePath = extractFilePath(request);

        if (filePath == "/") 
        {
            response = generateLoginPageResponse();
        } 
        else if (filePath == "/welcome") 
        {
            response = generateWelcomeResponse();
        } 
        else 
        {
            response = generateFileResponse(filePath);
        }
    } 
    else 
    {
        response = "HTTP/1.1 400 Bad Request\nContent-Length: 0\n\n";
    }

    send(client_socket, response.c_str(), response.size(), 0);
    close(client_socket);
}

std::string HttpServer::extractPostBody(const std::string& request) 
{
    std::size_t pos = request.find("\r\n\r\n");
    if (pos != std::string::npos) 
    {
        return request.substr(pos + 4); 
    }
    return "";
}

std::string HttpServer::extractParameter(const std::string& body, const std::string& param) 
{
    std::size_t start = body.find(param + "=");
    if (start != std::string::npos) 
    {
        start += param.length() + 1; 
        std::size_t end = body.find("&", start);
        return body.substr(start, end - start);
    }
    return "";
}

std::string HttpServer::extractFilePath(const std::string& request) 
{
    std::size_t start = request.find("GET ") + 4;
    std::size_t end = request.find(" HTTP");
    return request.substr(start, end - start);
}

std::string HttpServer::generateFileResponse(const std::string& filePath) 
{
    std::ifstream file(filePath);
    if (!file) 
    {
        return "HTTP/1.1 404 Not Found\nContent-Length: 0\n\n";
    }

    std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return "HTTP/1.1 200 OK\nContent-Length: " + std::to_string(fileContent.size()) + "\n\n" + fileContent;
}

std::string HttpServer::generateWelcomeResponse() 
{
    std::string welcomeContent =
        "<html>"
        "<body>"
        "<h1><strong>Welcome to the GXS`s Web Server!</strong></h1>"
        "<p>Hello JoTang!.</p>"
        "<p>I'm glad you can see this, that means my project is a success!</p>"
        "<p>Let's meet at the interview.</p>"
        "<p><strong>(So please let me through!)</strong></p>"
        "</body>"
        "</html>";
    return "HTTP/1.1 200 OK\nContent-Length: " + std::to_string(welcomeContent.size()) + "\n\n" + welcomeContent;
}

std::string HttpServer::generateLoginPageResponse() 
{
    std::string loginContent =
        "<html>"
        "<head><title>登录</title></head>"
        "<body>"
        "<h1>登录</h1>"
        "<form action=\"/login\" method=\"post\">"
        "<label for=\"username\">用户名:</label>"
        "<input type=\"text\" id=\"username\" name=\"username\" required>"
        "<br>"
        "<label for=\"password\">密码:</label>"
        "<input type=\"password\" id=\"password\" name=\"password\" required>"
        "<br>"
        "<button type=\"submit\">登录</button>"
        "</form>"
        "</body>"
        "</html>";
    return "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: " + std::to_string(loginContent.size()) + "\n\n" + loginContent;
}

void HttpServer::start() 
{
    ThreadPool pool(4);
    while (true) 
    {
        int client_socket = accept(server_fd, nullptr, nullptr);
        pool.enqueue([this, client_socket] 
        {
            this->handleClient(client_socket);
        });
    }
}
