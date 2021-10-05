#pragma once
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>

class UdpSocket
{
private:
    static size_t const _kBufferSize = 512;
    sockaddr_in _addr;
    int _socket;
    int _addr_sz = sizeof(_addr);
    char _buffer[_kBufferSize];
    WSADATA _wsa;

    std::string throwWsaError(std::string const& msg);
    UdpSocket();

public:
    UdpSocket(std::string const& ip_addr, u_short port);
    UdpSocket(sockaddr_in const& addr_info);
    UdpSocket(UdpSocket const& sock);
    ~UdpSocket();
    int recv(char* result, size_t& sz);
    int send(char const* msg, size_t sz);
};