#pragma once

#include <crow/include/crow.h>

#include <string>
#include <map>

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
		Server(short aPort);
		void Initialize(Blockchain& aBlockChain);
		void Run();

		std::string HttpGet(const std::string& anAddress);
		std::string HttpPost(const std::string& anAddress, const std::string& aBody, const std::map<std::string, std::string>& someHeaders);
		std::string GetMyHttpAdress();

	private:
		crow::SimpleApp mApp;
		std::string mNodeIdentifier;
		short mPort{};
	};
}

