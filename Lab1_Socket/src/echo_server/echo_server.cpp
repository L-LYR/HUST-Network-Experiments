#include "echo_server.hpp"

#include <iostream>
#include <string>

#include "config.hpp"

echoServer::echoServer(u_short port, const std::string& addr) {
    _clientSock = INVALID_SOCKET;
    ZeroMemory(&_clientAddr, sizeof(_clientAddr));
    _serverSock = INVALID_SOCKET;
    ZeroMemory(&_serverAddr, sizeof(_serverAddr));
    _iMode = 1;  // non-blocking
    _port = port;
    _saddr = addr;
}

bool echoServer::run() {
    if (!setup()) {
        std::cout << "Setting up server failed!" << std::endl;
        return false;
    }
    int clientAddrSize = sizeof(_clientAddr);
    int nBytesRecv;

    fd_set rfds;  // read file-discriptor set

    while (true) {
        // empty file-discriptor set
        FD_ZERO(&rfds);
        // set listening socket waiting for connection
        FD_SET(_serverSock, &rfds);
        // set connection socket readable and writable
        if (_clientSock != INVALID_SOCKET) {
            FD_SET(_clientSock, &rfds);
        }
        // selecting
        int ret = 0;
        if ((ret = select(0, &rfds, NULL, NULL, NULL)) < 0) {
            std::cout << "Current work finished..." << std::endl;
            cleanup();
            return true;
        }
        // make the connection
        if (FD_ISSET(_serverSock, &rfds)) {  // listening socket get connection request
            ret--;
            _clientSock = ::accept(_serverSock, (sockaddr*)&_clientAddr, &clientAddrSize);
            if (_clientSock == INVALID_SOCKET) {
                std::cout << "Making connection failed!" << std::endl;
                break;
            }
            if (ioctlsocket(_clientSock, FIONBIO, &_iMode) == SOCKET_ERROR) {
                std::cout << "Setting connection socket mode failed!" << std::endl;
                break;
            }
            std::cout << "Connection from " << inet_ntoa(_clientAddr.sin_addr)
                      << ":" << ntohs(_clientAddr.sin_port) << std::endl;
            FD_SET(_clientSock, &rfds);
        }

        if (ret > 0 && FD_ISSET(_clientSock, &rfds)) {
            ZeroMemory(recvBuf, BUFFER_SIZE);
            nBytesRecv = ::recv(_clientSock, recvBuf, BUFFER_SIZE, 0);
            // std::cout << WSAGetLastError() << std::endl;
            if (nBytesRecv < 0) {
                // std::cout << "Recving data failed!" << std::endl;
                // break;
                // do nothing
            } else if (nBytesRecv == 0) {
                std::cout << "Connection closed..." << std::endl;
                closesocket(_clientSock);
                _clientSock = INVALID_SOCKET;
            } else {
                std::cout << "Received " << nBytesRecv << " bytes of data." << std::endl;
                std::cout << "Detail: " << recvBuf << std::endl;
                // echo
                if (::send(_clientSock, recvBuf, nBytesRecv, 0) == SOCKET_ERROR) {
                    std::cout << "Sending data failed!" << std::endl;
                    break;
                } else {
                    std::cout << "Echo..." << std::endl;
                }
            }
        }
    }

    std::cout << "An error occurred during running." << std::endl
              << "Exit..." << std::endl;
    cleanup();
    return false;
}

bool echoServer::setup() {
    WSAData wsa;
    if (WSAStartup(WS_VER, &wsa) != 0) {
        std::cout << "WSA startup failed!" << std::endl;
        return false;
    }

    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_port = htons(_port);
    if (_saddr == "") {
        _serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    } else if (_saddr == "localhost") {
        char hostname[256];
        gethostname(hostname, sizeof(hostname));
        // std::cout << hostname << std::endl;
        hostent* tpHostent = gethostbyname(hostname);
        memcpy(&_serverAddr.sin_addr.S_un.S_addr,
               tpHostent->h_addr_list[0], tpHostent->h_length);
    } else {
        _serverAddr.sin_addr.S_un.S_addr = inet_addr(_saddr.c_str());
    }

    // only support TCP for this experiment
    _serverSock = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_serverSock == INVALID_SOCKET) {
        std::cout << "Creating listening socket failed!" << std::endl;
        return false;
    }

    // bind
    if (::bind(_serverSock, (sockaddr*)&_serverAddr, sizeof(_serverAddr)) == SOCKET_ERROR) {
        std::cout << "Binding server address failed!" << std::endl;
        return false;
    }

    // listen
    if (::listen(_serverSock, BACKLOG_SIZE) == SOCKET_ERROR) {
        std::cout << "Listening failed!" << std::endl;
        return false;
    }

    // set IO mode
    if (ioctlsocket(_serverSock, FIONBIO, &_iMode) == SOCKET_ERROR) {
        std::cout << "Setting listening socket mode failed!" << std::endl;
        return false;
    }

    std::cout << "Running..." << std::endl;
    showServerInfo();
    return true;
}

void echoServer::cleanup() {
    if (closesocket(_serverSock) == SOCKET_ERROR) {
        std::cout << "Close error" << std::endl;
    };
    WSACleanup();
}

void echoServer::showServerInfo() {
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "Server Type: single-thread TCP echo server" << std::endl;
    std::cout << "Address: " << inet_ntoa(_serverAddr.sin_addr)
              << ":" << ntohs(_serverAddr.sin_port) << std::endl;
    std::cout << "------------------------------------------" << std::endl;
}