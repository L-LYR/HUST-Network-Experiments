#ifndef ECHO_SERVER_HPP
#define ECHO_SERVER_HPP

#include <string>

#include "config.hpp"

// single-thread TCP echo server
class echoServer {
   public:
    echoServer(u_short port) : echoServer(port, "") {}
    echoServer(u_short port, const std::string &addr);

    ~echoServer(){};
    bool run();

   protected:
    bool setup();
    void cleanup();
    void showServerInfo();

   protected:
    socket_t _clientSock;     // single thread
    sockaddr_in _clientAddr;  // client address

    socket_t _serverSock;     // listening socket
    sockaddr_in _serverAddr;  // server address

    u_long _iMode;  // socket mode, non-blocking in this experiment
    u_short _port;
    std::string _saddr;

    enum {
        BUFFER_SIZE = 4096
    };
    char recvBuf[BUFFER_SIZE];
};

#endif