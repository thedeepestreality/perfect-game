/*
	Simple UDP Server
*/

#include <iostream>
#include <memory>
#include "UdpListener.h"

// Winsock Library
#pragma comment(lib,"ws2_32.lib")

//ip address of udp server
std::string const kIpAddr = "127.0.0.1";
// The port on which to listen for incoming data
size_t const kPort = 8888;

int main()
{
	std::unique_ptr<UdpListener> sock_listen;
	try
	{
		sock_listen = std::make_unique<UdpListener>(kIpAddr, kPort);
	}
	catch (std::exception const& err)
	{
		std::cout << "Couldn't init socket: " << err.what() << "\n";
		exit(EXIT_FAILURE);
	}
	std::cout << "Bind done\n";
	std::string msg;

	// keep listening for data
	while (1)
	{
		std::cout << "\nWaiting for data...\n";
		std::unique_ptr<UdpSocket> sock;
		try
		{
			sock = std::make_unique<UdpSocket>(sock_listen->recv(msg));
		}
		catch (std::exception const& err)
		{
			std::cout << "Socket recv error : " << err.what() << "\n";
			exit(EXIT_FAILURE);
		}

		std::cout << "Received data : " << msg << "\n";

		// now reply the client with the same data
		if (sock->send(msg.c_str(), msg.length()) != 0)
		{
			std::cout << "Failed to send\n";
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}

//auto start = std::chrono::high_resolution_clock::now();
//auto finish = std::chrono::high_resolution_clock::now();
//auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
