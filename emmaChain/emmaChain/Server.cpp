#include "Server.h"

#include "Blockchain.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/URI.h>


namespace emmaChain {
	void WriteTransactionAsJsonResponse(crow::json::wvalue& aJsonResponse, const Transaction& aTransaction, size_t aTransactionIndex, bool aWriteInChain, size_t aBlockIndex = 0)
	{
		if (!aWriteInChain)
		{
			aJsonResponse["transactions"][aTransactionIndex]["sender"] = aTransaction.mSender;
			aJsonResponse["transactions"][aTransactionIndex]["recipient"] = aTransaction.mRecipient;
			aJsonResponse["transactions"][aTransactionIndex]["amount"] = aTransaction.mAmount;
			aJsonResponse["transactions"][aTransactionIndex]["message"] = aTransaction.mMessage;
		}
		else
		{
			aJsonResponse["chain"][aBlockIndex]["transactions"][aTransactionIndex]["sender"] = aTransaction.mSender;
			aJsonResponse["chain"][aBlockIndex]["transactions"][aTransactionIndex]["recipient"] = aTransaction.mRecipient;
			aJsonResponse["chain"][aBlockIndex]["transactions"][aTransactionIndex]["amount"] = aTransaction.mAmount;
			aJsonResponse["chain"][aBlockIndex]["transactions"][aTransactionIndex]["message"] = aTransaction.mMessage;
		}
	}

	void WriteChainAsJsonResponse(crow::json::wvalue& aJsonResponse, const Blockchain& aBlockchain)
	{
		const auto& chain = aBlockchain.GetChain();
		aJsonResponse["length"] = chain.size();
		size_t blockIndex = 0;
		for (const auto& block : chain)
		{
			aJsonResponse["chain"][blockIndex]["index"] = block.GetIndex();
			aJsonResponse["chain"][blockIndex]["previous_hash"] = block.GetPreviousHash();
			aJsonResponse["chain"][blockIndex]["timestamp"] = block.GetTimestamp();
			aJsonResponse["chain"][blockIndex]["proof"] = block.GetProof();
			const auto& transactions = block.GetTransactions();
			size_t transactionIndex = 0;
			for (const auto& transaction : transactions)
			{
				WriteTransactionAsJsonResponse(aJsonResponse, transaction, transactionIndex, true, blockIndex);
				transactionIndex++;
			}
			blockIndex++;
		}
	}

	void Server::Initialize(Blockchain& aBlockchain)
	{
		boost::uuids::uuid uuid = boost::uuids::random_generator()();
		std::string nodeIdentifier = boost::uuids::to_string(uuid);

		CROW_ROUTE(mApp, "/mine")([&, nodeIdentifier]() {
			const auto& lastBlock = aBlockchain.GetLastBlock();
			int64_t lastProof = lastBlock.GetProof();
			int64_t proof = aBlockchain.ProofOfWork(lastProof);
			aBlockchain.NewTransaction("0", nodeIdentifier, "Mining reward", 1);
			std::string previousHash = aBlockchain.Hash(lastBlock);
			emmaChain::Block block = aBlockchain.NewBlock(proof, previousHash);

			crow::json::wvalue jsonResponse;
			jsonResponse["message"] = "New Block Forged";
			jsonResponse["index"] = block.GetIndex();
			jsonResponse["proof"] = block.GetProof();
			jsonResponse["previous_hash"] = block.GetPreviousHash();

			const auto& transactions = block.GetTransactions();
			size_t transactionIndex = 0;
			for (const auto& transaction : transactions)
			{
				WriteTransactionAsJsonResponse(jsonResponse, transaction, transactionIndex, false);
				transactionIndex++;
			}
			return jsonResponse;
			});

		CROW_ROUTE(mApp, "/chain")([&]() {
			crow::json::wvalue jsonResponse;
			WriteChainAsJsonResponse(jsonResponse, aBlockchain);
			return jsonResponse;
			});

		CROW_ROUTE(mApp, "/transactions/new")
			.methods("POST"_method)
			([&](const crow::request& aRequest) {
			auto values = crow::json::load(aRequest.body);
			if (!values) {
				return crow::response(400);
			}
			std::array<std::string, 4> requiredValues{ "sender", "recipient", "message", "amount" };
			for (const auto& requiredValue : requiredValues)
			{
				if (!values.has(requiredValue))
				{
					return crow::response(400, "Required value is missing");
				}
			}

			int index = aBlockchain.NewTransaction(values["sender"].s(), values["recipient"].s(), values["message"].s(), values["amount"].i());
			return crow::response{ "Transaction will be added to Block: " + std::to_string(index) };
				});

		CROW_ROUTE(mApp, "/nodes/register")
			.methods("POST"_method)
			([&](const crow::request& aRequest) {
			auto values = crow::json::load(aRequest.body);
			if (!values)
			{
				return crow::response(400);
			}
			auto nodes = values["nodes"];
			if (nodes.size() < 1)
			{
				return crow::response(400, "Error: Please supply a valid list of nodes");
			}
			for (auto& node : nodes)
			{
				aBlockchain.RegisterNode(node.s());
			}

			crow::json::wvalue jsonResponse;
			jsonResponse["message"] = "New nodes have been added";
			const auto& allNodes = aBlockchain.GetNodes();
			size_t nodeIndex = 0;
			for (const auto& node : allNodes)
			{
				jsonResponse["total_nodes"][nodeIndex] = node;
				nodeIndex++;
			}

			return crow::response(jsonResponse);
				});

		CROW_ROUTE(mApp, "/nodes/resolve")([&]() {
			crow::json::wvalue jsonResponse;
			bool replaced = aBlockchain.ResolveConflicts();
			jsonResponse["message"] = replaced ? "Our chain was replaced" : "Our chain is authoritative";
			WriteChainAsJsonResponse(jsonResponse, aBlockchain);
			return jsonResponse;
			});
	}

	void Server::Run(int aPort)
	{
		mApp.port(aPort).multithreaded().run();
	}

	std::string Server::HttpGet(const std::string& anAddress)
	{
		Poco::URI uri(anAddress);
		std::string path(uri.getPathAndQuery());
		if (path.empty()) {
			path = "/";
		}

		Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1);
		Poco::Net::HTTPResponse response;

		session.sendRequest(request);
		std::istream& responseStream = session.receiveResponse(response);
		std::cout << response.getStatus() << " " << response.getReason() << std::endl;
		std::string responseString;
		if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK)
		{
			return responseString;
		}

		std::ostringstream os;
		os << responseStream.rdbuf();
		responseString = os.str();
		return responseString;
	}
}
