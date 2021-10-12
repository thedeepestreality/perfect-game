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
	std::string name = "ping";
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
	//send the message
	if (sock_ptr->send(name.c_str(), name.length()) != 0)
	{
		std::cout << "Failed to send\n";
		exit(EXIT_FAILURE);
	}

	std::cout << "request sent\n";
	while (1)
	{
		//receive a reply and print it
		size_t sz = kBufferSize;
		if (sock_ptr->recv(buffer, sz) != 0)
		{
			std::cout << "Failed to recv\n";
			exit(EXIT_FAILURE);
		}

		std::cout << "Received game state: " << sz << "\n";
		state.deserialize(buffer, sz);
		GameState::PlayerPos curr_pos = state.getPlayerPos(name);
		std::cout << "Received player pos: (";
		std::cout << (int)curr_pos.first << "," << (int)curr_pos.second << ")\n";
		++curr_pos.second;
		sz = kBufferSize;
		state.serialize_player(buffer, sz, name, curr_pos);

		if (sock_ptr->send(buffer, sz) != 0)
		{
			std::cout << "Failed to send pos\n";
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}