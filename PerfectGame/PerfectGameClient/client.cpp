/*
	Simple udp client
*/
#include <iostream>
#include <memory>
#include <chrono>
#include "UdpSocket.h"
#include "../GameState/GameState.h"

//ip address of udp server
std::string const kIpAddr = "127.0.0.1";
//The port on which to listen for incoming data
u_short const kPort = 8888;
size_t const kBufferSize = 512;
char buffer[kBufferSize];

void sleep(unsigned long us)
{
	auto start = std::chrono::high_resolution_clock::now();
	auto finish = std::chrono::high_resolution_clock::now();
	auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
	while (microseconds.count() < us)
	{
		finish = std::chrono::high_resolution_clock::now();
		microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
	}
}

int main(int argc, const char* argv[])
{
	std::unique_ptr<UdpSocket> sock_ptr;
	std::string name = argv[1];
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
			std::cout << "No data to recv\n";
			sleep(1e5);
			continue;
			//exit(EXIT_FAILURE);
		}

		std::cout << "Received game state: " << sz << "\n";
		state.deserialize(buffer, sz);
		Player* p = state.getPlayer(name);
		PlayerPos curr_pos = p->getPos();
		std::cout << "Received player pos: (";
		std::cout << (int)curr_pos.first << "," << (int)curr_pos.second << ")\n";
		++curr_pos.second;
		p->updatePos(curr_pos.first, curr_pos.second);
		sz = kBufferSize;
		p->serialize(buffer, sz);

		if (sock_ptr->send(buffer, sz) != 0)
		{
			std::cout << "Failed to send pos\n";
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}