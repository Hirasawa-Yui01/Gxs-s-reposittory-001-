#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <cstring>

#define PORT 8080
#define BUFFER_SIZE 1024
#define ROOT_DIR "/home/yui/cproject/jotang/task2/Test folder" 

const char* get_mime_type(const std::string& path) 
{
    size_t dot_pos = path.find_last_of(".");
    if (dot_pos == std::string::npos) return "text/plain";
    
    std::string ext = path.substr(dot_pos);
    if (ext == ".html") return "text/html";
    if (ext == ".css") return "text/css";
    if (ext == ".js") return "application/javascript";
    if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
    if (ext == ".png") return "image/png";
    if (ext == ".gif") return "image/gif";
    return "text/plain";
}

void send_response(int client_socket, const std::string& header, const std::string& content_type, const std::string& body) 
{
    std::ostringstream response;
    response << header
             << "Content-Type: " << content_type << "\r\n"
             << "Content-Length: " << body.size() << "\r\n"
             << "\r\n"
             << body;

    send(client_socket, response.str().c_str(), response.str().size(), 0);
}

void handle_request(int client_socket, const std::string& request) 
{
    std::string method, path;
    std::istringstream request_stream(request);

    request_stream >> method >> path;

    while (path.find("..") != std::string::npos) 
    {
        path.replace(path.find(".."), 2, "");
    }

    std::string full_path = std::string(ROOT_DIR) + path;

    struct stat path_stat;
    if (stat(full_path.c_str(), &path_stat) == 0 && S_ISDIR(path_stat.st_mode)) 
    {
        full_path += "/index.html";
    }

    std::ifstream file(full_path, std::ios::binary);
    if (!file.is_open()) 
    {
        std::string not_found_body = "<html><body><h1>404 Not Found</h1></body></html>";
        send_response(client_socket, "HTTP/1.1 404 Not Found\r\n", "text/html", not_found_body);
        return;
    }

    std::ostringstream file_content;
    file_content << file.rdbuf();
    file.close();

    send_response(client_socket, "HTTP/1.1 200 OK\r\n", get_mime_type(full_path), file_content.str());
}

int main() 
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) 
    {
        std::cerr << "Socket creation failed. Error Code : " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
    {
        std::cerr << "Bind failed. Error Code : " << errno << std::endl;
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 3) < 0) 
    {
        std::cerr << "Listen failed. Error Code : " << errno << std::endl;
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    std::cout << "HTTP Server is running at http://127.0.0.1:" << PORT << "/\n";

    int client_socket;
    sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    while ((client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_addr_len)) >= 0) 
    {
        char request_buffer[BUFFER_SIZE] = {0};
        recv(client_socket, request_buffer, BUFFER_SIZE, 0);

        handle_request(client_socket, request_buffer);

        close(client_socket);
    }

    close(server_socket);
    return 0;
}
