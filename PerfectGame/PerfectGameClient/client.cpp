/*
	Simple udp client
*/
#include <iostream>
#include <memory>
#include "UdpSocket.h"

//Winsock Library
#pragma comment(lib,"ws2_32.lib")

//ip address of udp server
std::string const kIpAddr = "127.0.0.1";
//The port on which to listen for incoming data
u_short const kPort = 8888;

int main()
{
	std::unique_ptr<UdpSocket> sock_ptr;
	std::string message;

	try
	{
		sock_ptr = std::make_unique<UdpSocket>(kIpAddr, kPort);
	}
	catch (std::exception const& err)
	{
		std::cout << "Couldn't init socket: " << err.what() << "\n";
		exit(EXIT_FAILURE);
	}

	//start communication
	while (1)
	{
		std::cout<<"Enter message : ";
		std::cin >> message;

		//send the message
		if (sock_ptr->send(message) != 0)
		{
			std::cout << "Failed to send\n";
			exit(EXIT_FAILURE);
		}

		//receive a reply and print it
		if (sock_ptr->recv(message) != 0)
		{
			std::cout << "Failed to recv\n";
			exit(EXIT_FAILURE);
		}

		std::cout << "Receive msg: " << message << "\n";
	}

	return 0;
}