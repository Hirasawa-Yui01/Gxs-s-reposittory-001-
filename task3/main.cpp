#include "HttpServer.h"

int main() 
{
    HttpServer server(8080);
    server.start();
    return 0;
}
