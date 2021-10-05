/*
	Simple udp client
*/
#include <iostream>
#include <memory>
#include "UdpSocket.h"
#include "../GameState/GameState.h"

//Winsock Library
#pragma comment(lib,"ws2_32.lib")

//ip address of udp server
std::string const kIpAddr = "127.0.0.1";
//The port on which to listen for incoming data
u_short const kPort = 8888;
size_t const kBufferSize = 512;
char buffer[kBufferSize];

int main()
{
	std::unique_ptr<UdpSocket> sock_ptr;
	std::string message = "ping";
	GameState state;

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
	//while (1)
	//{
		//std::cout<<"Enter message : ";
		//std::cin >> message;

		//send the message
		if (sock_ptr->send(message.c_str(), message.length()) != 0)
		{
			std::cout << "Failed to send\n";
			exit(EXIT_FAILURE);
		}

		std::cout << "request sent\n";

		//receive a reply and print it
		size_t sz = kBufferSize;
		if (sock_ptr->recv(buffer, sz) != 0)
		{
			std::cout << "Failed to recv\n";
			exit(EXIT_FAILURE);
		}

		std::cout << "Received game state: " << sz << "\n";
		state.deserialize(buffer, sz);
	//}

	return 0;
}