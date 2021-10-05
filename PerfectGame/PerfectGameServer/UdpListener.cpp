#include "UdpListener.h"
#include <stdexcept>

std::string UdpListener::throwWsaError(std::string const& msg)
{
	throw std::runtime_error(msg + std::to_string(WSAGetLastError()));
}

UdpListener::UdpListener(std::string const& ip_addr, u_short port)
{
	// Initialize winsock
	if (WSAStartup(MAKEWORD(2, 2), &_wsa) != 0)
		throwWsaError("WSAStartup failed. Error Code : ");
	// Create a socket
	if ((_socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
		throwWsaError("Could not create socket : ");

	unsigned long ul = 1;
	if (ioctlsocket(_socket, FIONBIO, &ul) == SOCKET_ERROR)
		throwWsaError("Could not set non-blocking mode");

	// Prepare the sockaddr_in structure
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(port);
	InetPton(AF_INET, ip_addr.c_str(), &_addr.sin_addr.s_addr);
	// Bind
	if (bind(_socket, (struct sockaddr*)&_addr, _addr_sz) == SOCKET_ERROR)
		throwWsaError("Bind failed with error code : ");
}

UdpListener::~UdpListener()
{
	closesocket(_socket);
	WSACleanup();
}

UdpSocket UdpListener::recv(std::string& result)
{
	memset(_buffer, '\0', _kBufferSize);
	//try to receive some data, this is a blocking call
	sockaddr_in _from_addr;
	if (recvfrom(_socket, _buffer, _kBufferSize, 0, (sockaddr*)&_from_addr, &_addr_sz) == SOCKET_ERROR)
		throwWsaError("Failed recv from socket, error code: ");
	result = std::string(_buffer);
	return UdpSocket(_from_addr);
}
