#include "Blockchain.h"
#include "Server.h"

#include <algorithm>
#include <iostream>
#include <thread>

int main() {
	std::thread thread1([]() {
		short port = 18080;
		emmaChain::Server server(port);
		emmaChain::Blockchain blockchain(server);

		server.Initialize(blockchain);
		server.Run();
		});

	std::thread thread2([]() {
		short port = 18081;
		emmaChain::Server server(port);
		emmaChain::Blockchain blockchain(server);

		server.Initialize(blockchain);
		server.Run();
		});

	std::cin.get();

	return 0;
}