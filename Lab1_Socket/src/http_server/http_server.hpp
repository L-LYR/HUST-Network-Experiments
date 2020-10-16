#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <string>

#include "config.hpp"

class httpServer {
   public:
    httpServer(u_short port,
               const std::string &addr = "",
               const std::string &serverName = "Single-thread HTTP Server",
               const std::string &mainPath = ".");
    ~httpServer() {
        cleanup();
    }

    void run();

   protected:
    bool setup();
    void cleanup();
    void showServerInfo();
    socket_t createConnection();

   private:
    std::string _name;  // server name
    std::string _path;  // main path

    socket_t _serverSock;     // listening socket
    sockaddr_in _serverAddr;  // server address

    u_short _port;
    std::string _saddr;

    enum {
        BUFFER_SIZE = 81920
    };
    char recvBuf[BUFFER_SIZE];
    char sendBuf[BUFFER_SIZE];
};

#endif