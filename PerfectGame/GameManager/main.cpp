#include <iostream>
#include <chrono>
#include "..\GameState\GameState.h"
#include "..\PerfectGameClient\UdpSocket.h"

// Winsock Library
#pragma comment(lib,"ws2_32.lib")

static size_t const kBufSize = 512;
static char buf[kBufSize];

//ip address of udp server
std::string const kIpAddr = "127.0.0.1";
// The port on which to listen for incoming data
size_t const kPort = 8888;

GameIdx kDefX = 0;
GameIdx kDefY = 0;

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

unsigned long elapsed_ms(std::chrono::steady_clock::time_point start)
{
	auto curr = std::chrono::high_resolution_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(curr - start);
	return diff.count();
}

unsigned long const kPeriod = 100; //ms

int main()
{
    GameState game;
    
	std::unique_ptr<UdpSocket> sock_listen;
	std::string message;
	size_t sz = kBufSize;
	try
	{
		sock_listen = std::make_unique<UdpSocket>(kPort);
	}
	catch (std::exception const& err)
	{
		std::cout << "Couldn't init socket: " << err.what() << "\n";
		exit(EXIT_FAILURE);
	}
	std::cout << "Bind done\n";
	std::string player_name;
	std::shared_ptr<UdpSocket> sock;
	// game cycle
	while (1)
	{
		game.incrementAll();
		auto start = std::chrono::high_resolution_clock::now();
		while(elapsed_ms(start) < kPeriod)
		{
			try
			{
				sz = kBufSize;
				int res = sock_listen->recv(buf, sz, sock);
				if (res != 0)
				{
					//recv nothing
					if (res == 10035)
						continue;
					//smth serious happend
					else
						throw std::runtime_error(std::string("error ") + std::to_string(res));
				}
				player_name = std::string(buf);
				std::cout << "Received data : " << player_name << "\n";
				Player* player_ptr;
				//TODO: what about socket if player re-enters
				if ((player_ptr = game.getPlayer(player_name)) == nullptr)
					game.addPlayer(player_name, sock, kDefX, kDefY);
				else
				{
					if (player_ptr->getStatus() == PlayerStatus::NotActive)
						player_ptr->activate();
					else
					{
						player_ptr->resetLossCounter();
						size_t str_sz = player_name.length();
						GameIdx x = buf[str_sz + 1];
						GameIdx y = buf[str_sz + 2];
						player_ptr->updatePos(x, y);
						if (sz > str_sz + 3)
						{
							x = buf[str_sz + 3];
							y = buf[str_sz + 4];
							GameState::Block block = (GameState::Block)buf[str_sz + 5];
						}
					}
				}
				break;
			}
			catch (std::exception const& err)
			{
				std::cout << "Socket recv error : " << err.what() << "\n";
			}
		}
		game.sendAll();
	}

    return 0;
}