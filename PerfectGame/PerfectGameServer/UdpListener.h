#pragma once

#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include "../PerfectGameClient/UdpSocket.h"

class UdpListener
{
private:
    static size_t const _kBufferSize = 512;
    sockaddr_in _addr;
    int _socket;
    int _addr_sz = sizeof(_addr);
    char _buffer[_kBufferSize];
    WSADATA _wsa;

    std::string throwWsaError(std::string const& msg);

public:
    UdpListener(std::string const& ip_addr = "127.0.0.1", u_short port = 8888);
    ~UdpListener();
    UdpSocket recv(std::string& result);
};