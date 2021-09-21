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

int UdpSocket::recv(std::string& result)
{
	memset(_buffer, '\0', _kBufferSize);
	//try to receive some data, this is a blocking call
	if (recvfrom(_socket, _buffer, _kBufferSize, 0, (sockaddr*)&_addr, &_addr_sz) == SOCKET_ERROR)
		return WSAGetLastError();
	result = std::string(_buffer);
	return 0;
}

int UdpSocket::send(std::string const& message)
{
	char const* msg_buff = message.c_str();
	if (sendto(_socket, msg_buff, strlen(msg_buff), 0, (sockaddr*)&_addr, _addr_sz) == SOCKET_ERROR)
		return WSAGetLastError();
	return 0;
}
