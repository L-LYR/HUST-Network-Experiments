#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <WS2tcpip.h>
#include <WinSock2.h>

#define WS_VER MAKEWORD(2, 2)  // WinSock Version
#define BACKLOG_SIZE SOMAXCONN

typedef SOCKET socket_t;
typedef TIMEVAL timeval_t;

#endif