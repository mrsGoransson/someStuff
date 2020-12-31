#pragma once

#include <crow/include/crow.h>

namespace Poco {
	namespace Net {
		class HTTPClientSession;
		class HTTPRequest;
		class HTTPResponse;
	}
}

namespace emmaChain {
	class Blockchain;

	class Server {
	public:
		void Initialize(Blockchain& aBlockChain);
		void Run(int aPort);

		std::string HttpGet(const std::string& anAddress);

	private:
		crow::SimpleApp mApp;
	};
}

