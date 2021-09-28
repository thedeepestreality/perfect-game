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
	std::string msg;
	std::unique_ptr<UdpSocket> sock;
	// keep listening for data
	while (1)
	{
		std::cout << "\nWaiting for data...\n";

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
	}

	while(1)
	{
		game.serialize(buf, kBufSize);
		// now reply the client with the same data
		if (sock->send(buf) != 0)
		{
			std::cout << "Failed to send\n";
			exit(EXIT_FAILURE);
		}
		auto start = std::chrono::high_resolution_clock::now();
		auto finish = std::chrono::high_resolution_clock::now();
		auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
		while (microseconds.count() < 1e6)
		{
			finish = std::chrono::high_resolution_clock::now();
			microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
		}
	}

    return 0;
}