#include "UdpSocket.h"
#include <stdexcept>

std::string UdpSocket::throwWsaError(std::string const& msg)
{
	throw std::runtime_error(msg + std::to_string(WSAGetLastError()));
}

UdpSocket::UdpSocket()
{
	// initialize winsock
	if (WSAStartup(MAKEWORD(2, 2), &_wsa) != 0)
		throwWsaError("Failed. Error Code : ");

	// create socket
	if ((_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
		throwWsaError("socket() failed with error code : ");
}

UdpSocket::UdpSocket(sockaddr_in const& addr_info) : UdpSocket()
{
	_addr = addr_info;
}

UdpSocket::UdpSocket(std::string const& ip_addr, u_short port) : UdpSocket()
{
	// setup address structure
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(port);
	InetPton(AF_INET, ip_addr.c_str(), &_addr.sin_addr.s_addr);
}

UdpSocket::UdpSocket(UdpSocket const& sock) : UdpSocket()
{
	_addr = sock._addr;
}

UdpSocket::~UdpSocket()
{
	closesocket(_socket);
	WSACleanup();
}

int UdpSocket::recv(char* result, size_t& sz)
{
	//memset(_buffer, '\0', _kBufferSize);
	memset(result, '\0', sz);
	sockaddr_in recv_addr;
	//try to receive some data, this is a blocking call
	if (sz = recvfrom(_socket, result, sz, 0, (sockaddr*)&recv_addr, &_addr_sz) == SOCKET_ERROR)
		return WSAGetLastError();
	return 0;
}

int UdpSocket::send(char const* msg, size_t sz)
{
	if (sendto(_socket, msg, sz, 0, (sockaddr*)&_addr, _addr_sz) == SOCKET_ERROR)
		return WSAGetLastError();
	return 0;
}
