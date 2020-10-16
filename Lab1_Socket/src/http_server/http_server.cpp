#include "http_server.hpp"


#include <iostream>
#include <string>
#include <vector>

#include "config.hpp"
#include "http_request.hpp"
#include "http_response.hpp"

httpServer::httpServer(u_short port,
                       const std::string &addr,
                       const std::string &serverName,
                       const std::string &mainPath) {
    _serverSock = INVALID_SOCKET;
    ZeroMemory(&_serverAddr, sizeof(_serverAddr));
    _name = serverName;
    _path = mainPath;
    _port = port;
    _saddr = addr;
}

void httpServer::run() {
    if (!setup()) {
        std::cout << "Setting up server failed!" << std::endl;
        return;
    }
    httpResponse resp(_path, _name);
    fd_set rfds;  // read file-discriptor set
    std::vector<socket_t> clients;

    while (true) {
        // empty file-discriptor set
        FD_ZERO(&rfds);
        // set listening socket waiting for connection
        FD_SET(_serverSock, &rfds);
        for (const auto &si : clients) {
            FD_SET(si, &rfds);
        }

        // selecting
        if (select(0, &rfds, NULL, NULL, NULL) < 0) {
            std::cout << "Current work finished..." << std::endl;
            return;
        }

        if (FD_ISSET(_serverSock, &rfds)) {
            socket_t conn = createConnection();
            if (conn == SOCKET_ERROR) {
                std::cout << "Creating connection failed!" << std::endl;
            } else {
                clients.push_back(conn);
            }
        }

        for (const auto &si : clients) {
            if (FD_ISSET(si, &rfds)) {
                ZeroMemory(recvBuf, BUFFER_SIZE);
                int nRecv = ::recv(si, recvBuf, BUFFER_SIZE, 0);
                if (nRecv > 0) {
                    std::cout << "Received " << nRecv << " bytes of data." << std::endl
                              << "Detail: " << recvBuf << std::endl;
                    httpRequest req(recvBuf);
                    resp.respondTo(req);
                    std::cout << "Response status line: " << resp._statusLine << std::endl;
                    std::cout << "Response header lines: " << resp._headerLines << std::endl;
                    std::cout << "Send " << resp.getLength() << " bytes of data." << std::endl;
                    if (::send(si, resp.fullResponse(), resp.getLength(), 0) != SOCKET_ERROR) {
                        std::cout << "Sending data Succeeded!" << std::endl;
                    } else {
                        std::cout << "Sending data failed!" << std::endl;
                    }
                }
            }
        }
    }
}

socket_t httpServer::createConnection() {
    sockaddr_in clientAddr;
    ZeroMemory(&clientAddr, sizeof(clientAddr));
    int clientAddrSize = sizeof(clientAddr);

    socket_t conn = ::accept(_serverSock, (sockaddr *)&clientAddr, &clientAddrSize);
    if (conn == SOCKET_ERROR) {
        return SOCKET_ERROR;
    } else {
        std::cout << "Connection from: " << inet_ntoa(clientAddr.sin_addr)
                  << ": " << clientAddr.sin_port << std::endl;
    }

    u_long iMode = 1;  // non-blocking in default
    if (ioctlsocket(conn, FIONBIO, &iMode) == SOCKET_ERROR) {
        std::cout << "Setting connection socket mode failed!" << std::endl;
    }
    std::cout << "Client Socket: " << conn << std::endl;
    return conn;
}

bool httpServer::setup() {
    WSAData wsa;
    u_long iMode = 1;  // non-blocking in default

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
        hostent *tpHostent = gethostbyname(hostname);
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
    if (::bind(_serverSock, (sockaddr *)&_serverAddr, sizeof(_serverAddr)) == SOCKET_ERROR) {
        std::cout << "Binding server address failed!" << std::endl;
        return false;
    }

    // listen
    if (::listen(_serverSock, BACKLOG_SIZE) == SOCKET_ERROR) {
        std::cout << "Listening failed!" << std::endl;
        return false;
    }

    // set IO mode
    if (ioctlsocket(_serverSock, FIONBIO, &iMode) == SOCKET_ERROR) {
        std::cout << "Setting listening socket mode failed!" << std::endl;
        return false;
    }

    std::cout << "Running..." << std::endl;
    showServerInfo();
    return true;
}

void httpServer::cleanup() {
    closesocket(_serverSock);
    WSACleanup();
}

void httpServer::showServerInfo() {
    std::cout << "------------------------------------------" << std::endl
              << "Server Name: " << _name << std::endl
              << "Address: " << inet_ntoa(_serverAddr.sin_addr)
              << ":" << _port << std::endl
              << "Main Path: " << _path << std::endl
              << "------------------------------------------" << std::endl;
}