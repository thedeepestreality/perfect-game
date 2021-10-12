#include <iostream>
#include <chrono>
#include "..\GameState\GameState.h"
#include "..\PerfectGameServer\UdpListener.h"
#include "..\PerfectGameClient\UdpSocket.h"

// Winsock Library
#pragma comment(lib,"ws2_32.lib")

static size_t const kBufSize = 512;
static char buf[kBufSize];

//ip address of udp server
std::string const kIpAddr = "127.0.0.1";
// The port on which to listen for incoming data
size_t const kPort = 8888;

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

int main()
{
    GameState game;
    
	std::unique_ptr<UdpListener> sock_listen;
	std::string message;

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
	std::string player_name;
	std::unique_ptr<UdpSocket> sock;
	// keep listening for data
	while (1)
	{
		std::cout << "\nWaiting for data...\n";
		try
		{
			sock = std::make_unique<UdpSocket>(sock_listen->recv(player_name));
			std::cout << "Received data : " << player_name << "\n";
			game.setPlayerPos(player_name, GameState::PlayerPos(1, 1));
			break;
		}
		catch (std::exception const& err)
		{
			std::cout << "Socket recv error : " << err.what() << "\n";
			//exit(EXIT_FAILURE);
		}
		sleep(1e6);
	}

	while(1)
	{
		size_t sz = kBufSize;
		game.serialize(buf, sz);
		// now reply the client with the same data
		if (sock->send(buf, sz) != 0)
		{
			std::cout << "Failed to send\n";
			exit(EXIT_FAILURE);
		}

		try
		{
			sz = kBufSize;
			sock = std::make_unique<UdpSocket>(sock_listen->recv(buf, sz));
			std::cout << "Received data : " << player_name << "\n";
		}
		catch (std::exception const& err)
		{
			std::cout << "Socket recv error : " << err.what() << "\n";
			continue;
			//exit(EXIT_FAILURE);
		}
		if (sz > 0)
		{
			player_name = std::string(buf);
			size_t str_sz = player_name.length();
			GameState::GameIdx player_x = buf[str_sz + 1];
			GameState::GameIdx player_y = buf[str_sz + 2];
			game.setPlayerPos(player_name, GameState::PlayerPos(player_x, player_y));
		}
		sleep(1e6);
	}

    return 0;
}