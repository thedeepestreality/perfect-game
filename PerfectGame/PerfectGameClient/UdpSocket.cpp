#include "UdpSocket.h"
#include <stdexcept>

UdpSocket::UdpSocket(std::string const& ip_addr, u_short port)
{
	// initialize winsock
	if (WSAStartup(MAKEWORD(2, 2), &_wsa) != 0)
	{
		std::string error_msg = std::string("Failed. Error Code : ") 
								+ std::to_string(WSAGetLastError());
		throw std::runtime_error(error_msg);
	}

	// create socket
	if ((_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		std::string error_msg = std::string("socket() failed with error code : ")
			+ std::to_string(WSAGetLastError());
		throw std::runtime_error(error_msg);;
	}

	// setup address structure
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(port);
	InetPton(AF_INET, ip_addr.c_str(), &_addr.sin_addr.s_addr);
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