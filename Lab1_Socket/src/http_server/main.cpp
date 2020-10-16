#include <conio.h>

#include <iostream>
#include <string>
#include <thread>

#include "http_protocol.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include "http_server.hpp"

void shutdownServer(httpServer* p) {
    int k;
    while (1) {
        if (kbhit()) {
            k = getch();
            if (0 == k) getch();
            if (27 == k) break;
        }
    }
    p->~httpServer();
    std::cout << "Exiting..." << std::endl;
}

int main(int argc, char* argv[]) {
    u_short port = 27015;
    std::string addr = "localhost";
    std::string path = ".";
    std::string name = "Single-thread HTTP Server";
    if (argc < 2) {
        std::cout << "Usage: http_server.exe [<port> <ipv4-addr> <path>]" << std::endl
                  << "       [port]: set the listening port" << std::endl
                  << "       [ipv4-addr]: set the address(dotted decimal notation or 'localhost' for auto-detection)" << std::endl
                  << "       [path]: local path of the server" << std::endl
                  << "       In default, the config will be [27015] [localhost] [.]" << std::endl;
    } else {
        port = std::stoi(std::string(argv[1]));
        addr = std::string(argv[2]);
        path = std::string(argv[3]);
    }
    httpServer testHTTPServer(port, addr, name, path);

    std::thread shutdownServerThread(shutdownServer, &testHTTPServer);
    shutdownServerThread.detach();

    testHTTPServer.run();

    return 0;
}