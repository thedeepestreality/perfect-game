/*
	Simple UDP Server
*/

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

// Winsock Library
#pragma comment(lib,"ws2_32.lib")

//ip address of udp server
std::string const kIpAddr = "127.0.0.1";
// Max length of buffer
size_t const kBuflen = 512;
// The port on which to listen for incoming data
size_t const kPort = 8888;

int main()
{
	SOCKET sock;
	sockaddr_in server_addr, client_addr;
	int slen, recv_len;
	char buf[kBuflen];
	WSADATA wsa;

	slen = sizeof(client_addr);

	// Initialize winsock
	std::cout << "Initializing Winsock...\n";
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		std::cout << "Failed. Error Code : " << WSAGetLastError() << "\n";
		exit(EXIT_FAILURE);
	}
	std::cout << "Initialized.\n";

	// Create a socket
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		std::cout << "Could not create socket : " << WSAGetLastError() << "\n";
	}
	std::cout << "Socket created.\n";

	// Prepare the sockaddr_in structure
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(kPort);
	InetPton(AF_INET, kIpAddr.c_str(), &server_addr.sin_addr.s_addr);

	// Bind
	if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		std::cout << "Bind failed with error code : " <<  WSAGetLastError() << "\n";
		exit(EXIT_FAILURE);
	}
	std::cout << "Bind done\n";

	// keep listening for data
	while (1)
	{
		std::cout << "\nWaiting for data...\n";

		// clear the buffer by filling nulls, it might have previously received data
		memset(buf, '\0', kBuflen);

		// try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(sock, buf, kBuflen, 0, (struct sockaddr*)&client_addr, &slen)) == SOCKET_ERROR)
		{
			std::cout << "recvfrom() failed with error code : " << WSAGetLastError();
			exit(EXIT_FAILURE);
		}

		// print details of the client/peer and the data received
		char addr_buf[16];
		inet_ntop(AF_INET, &client_addr.sin_addr, addr_buf, 16);
		std::cout << "Received packet from " << addr_buf;
		std::cout << ":" << ntohs(client_addr.sin_port) << "\n";
		std::cout << "Data: " << buf << "\n";

		// now reply the client with the same data
		if (sendto(sock, buf, recv_len, 0, (struct sockaddr*)&client_addr, slen) == SOCKET_ERROR)
		{
			std::cout << "sendto() failed with error code : " << WSAGetLastError();
			exit(EXIT_FAILURE);
		}
	}

	closesocket(sock);
	WSACleanup();

	return 0;
}