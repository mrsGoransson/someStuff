#include "Blockchain.h"
#include "Server.h"

#include <algorithm>
#include <iostream>
#include <thread>

int main() {
	std::thread thread1([]() {
		emmaChain::Server server;
		emmaChain::Blockchain blockchain(server);

		server.Initialize(blockchain);
		server.Run(18080);	
		});

	std::thread thread2([]() {
		emmaChain::Server server;
		emmaChain::Blockchain blockchain(server);

		server.Initialize(blockchain);
		server.Run(18081);	
		});

	std::cin.get();

	return 0;
}