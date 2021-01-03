#include "Server.h"

#include "Blockchain.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <Poco/Net/DNS.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/StreamCopier.h>
#include <Poco/URI.h>
#include <rapidjson/document.h>


namespace emmaChain {
	void WriteTransactionAsJsonResponse(crow::json::wvalue& aJsonResponse, const Transaction& aTransaction, unsigned int aTransactionIndex, bool aWriteInChain, unsigned int aBlockIndex = 0)
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
		unsigned int blockIndex = 0;
		for (const auto& block : chain)
		{
			aJsonResponse["chain"][blockIndex]["index"] = block.GetIndex();
			aJsonResponse["chain"][blockIndex]["previous_hash"] = block.GetPreviousHash();
			aJsonResponse["chain"][blockIndex]["timestamp"] = block.GetTimestamp();
			aJsonResponse["chain"][blockIndex]["proof"] = block.GetProof();
			const auto& transactions = block.GetTransactions();
			unsigned int transactionIndex = 0;
			for (const auto& transaction : transactions)
			{
				WriteTransactionAsJsonResponse(aJsonResponse, transaction, transactionIndex, true, blockIndex);
				transactionIndex++;
			}
			blockIndex++;
		}
	}

	Server::Server(short aPort) : mPort(aPort)
	{
		boost::uuids::uuid uuid = boost::uuids::random_generator()();
		mNodeIdentifier = boost::uuids::to_string(uuid);
	}

	void Server::Initialize(Blockchain& aBlockchain) //TODO: refactor this 
	{
		CROW_ROUTE(mApp, "/block/add")
			.methods("POST"_method)
			([&](const crow::request& aRequest) {
			auto values = crow::json::load(aRequest.body);
			if (!values)
			{
				return crow::response(400);
			}

			std::vector<Transaction> blockTransactions;
			auto transactionsJson = values["transactions"];
			for (const auto& transaction : transactionsJson)
			{
				blockTransactions.push_back(Transaction{ transaction["sender"].s(),
																	transaction["recipient"].s(),
																	transaction["message"].s(),
																	static_cast<uint32_t>(transaction["amount"].u()) });
			}
			Block newBlock(values["index"].u(), values["proof"].i(), values["previous_hash"].s(),
				blockTransactions, values["timestamp"].i());

			bool result = aBlockchain.AddBlock(newBlock);
			if (!result)
			{
				return crow::response{ 400, "The block was discarded by the node" };
			}
			return crow::response{ "Block added to the chain" };
				});


		CROW_ROUTE(mApp, "/mine")([&]() {
			crow::json::wvalue jsonResponse;
			bool result = aBlockchain.Mine(mNodeIdentifier);
			if (!result)
			{
				jsonResponse["message"] = "No transactions to mine";
				return jsonResponse;
			}

			const auto& lastBlock = aBlockchain.GetLastBlock();
			bool replaced = aBlockchain.ResolveConflicts();
			if (!replaced)
			{
				std::string requestBody("{");
				requestBody += "\"index\":" + std::to_string(lastBlock.GetIndex()) + ",";
				requestBody += "\"previous_hash\":\"" + lastBlock.GetPreviousHash() + "\",";
				requestBody += "\"timestamp\":" + std::to_string(lastBlock.GetTimestamp()) + ",";
				requestBody += "\"proof\":" + std::to_string(lastBlock.GetProof()) + ",";
				requestBody += "\"transactions\":[";
				unsigned int transactionIndex = 0;
				for (const auto& transaction : lastBlock.GetTransactions())
				{
					requestBody += "{";
					requestBody += "\"sender\":\"" + transaction.mSender + "\",";
					requestBody += "\"recipient\":\"" + transaction.mRecipient + "\",";
					requestBody += "\"message\":\"" + transaction.mMessage + "\",";
					requestBody += "\"amount\":" + std::to_string(transaction.mAmount);
					requestBody += "},";
					transactionIndex++;
				}
				requestBody.pop_back();
				requestBody += "]}";

				const auto& otherNodes = aBlockchain.GetNodes();
				for (const auto& node : otherNodes)
				{
					HttpPost("http://" + node + "/block/add", requestBody, { {"content_type" , "application/json"} });
				}
			}

			jsonResponse["message"] = "Block " + std::to_string(lastBlock.GetIndex()) + " has been mined";
			jsonResponse["index"] = lastBlock.GetIndex();
			jsonResponse["proof"] = lastBlock.GetProof();
			jsonResponse["previous_hash"] = lastBlock.GetPreviousHash();
			const auto& transactions = lastBlock.GetTransactions();
			unsigned int transactionIndex = 0;
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
			unsigned int nodeIndex = 0;
			for (const auto& node : allNodes)
			{
				jsonResponse["total_nodes"][nodeIndex] = "http://" + node;
				nodeIndex++;
			}
			WriteChainAsJsonResponse(jsonResponse, aBlockchain);
			return crow::response(jsonResponse);
				});


		CROW_ROUTE(mApp, "/nodes/register/with")
			.methods("POST"_method)
			([&](const crow::request& aRequest) {
			auto values = crow::json::load(aRequest.body);
			if (!values)
			{
				return crow::response(400);
			}
			auto nodesToAdd = values["nodes"];
			if (nodesToAdd.size() < 1)
			{
				return crow::response(400, "Error: Please supply a valid list of nodes");
			}

			std::string body("{\"nodes\": ");
			unsigned int nodeIndex = 0;
			body += "[";
			auto thisHost = Poco::Net::DNS::thisHost();
			std::string myIpv4 = thisHost.addresses().back().toString();
			std::string myAddress = myIpv4 + ":" + std::to_string(mPort);
			body += "\"" + GetMyHttpAdress() + "\"";
			body += "]}";

			for (const auto& node : nodesToAdd)
			{
				std::string response = HttpPost(std::string(node.s()) + std::string("/nodes/register"), body, { {"content_type" , "application/json"} });
				if (!response.empty())
				{
					rapidjson::Document jsonDocument;
					jsonDocument.Parse(response.c_str());
					std::vector<Block> chainResponse = Blockchain::ConstructChainFromJson(jsonDocument);
					if (aBlockchain.ValidChain(chainResponse))
					{
						aBlockchain.OverwriteLocalChain(chainResponse);
					}
					if (jsonDocument.HasMember("total_nodes") && jsonDocument["total_nodes"].IsArray())
					{
						const auto& totalNodes = jsonDocument["total_nodes"].GetArray();
						for (const auto& registeredNode : totalNodes)
						{
							aBlockchain.RegisterNode(registeredNode.GetString());
						}
					}
				}
				else
				{
					return crow::response(400, "Error: Node list is not correct");
				}			
			}
			return crow::response{ "Registration successful" };

				});


		CROW_ROUTE(mApp, "/nodes/resolve")([&]() {
			crow::json::wvalue jsonResponse;
			bool replaced = aBlockchain.ResolveConflicts();
			jsonResponse["message"] = replaced ? "Our chain was replaced" : "Our chain is authoritative";
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

			int index = aBlockchain.NewTransaction(values["sender"].s(), values["recipient"].s(), values["message"].s(), static_cast<uint32_t>(values["amount"].u()));
			return crow::response{ "Transaction will be added to Block: " + std::to_string(index) };
				});


		CROW_ROUTE(mApp, "/transactions/pending")([&]() {
			crow::json::wvalue jsonResponse;
			const auto& pendingTransacations = aBlockchain.GetPendingTransactions();
			unsigned int transactionIndex = 0;
			for (const auto& transaction : pendingTransacations)
			{
				WriteTransactionAsJsonResponse(jsonResponse, transaction, transactionIndex, false);
				transactionIndex++;
			}
			return jsonResponse;
			});

	}

	void Server::Run()
	{
		mApp.port(mPort).multithreaded().run();
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
		if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK)
		{
			return "";
		}

		std::stringstream ss;
		Poco::StreamCopier::copyStream(responseStream, ss);
		return ss.str();
	}

	std::string Server::HttpPost(const std::string& anAddress, const std::string& aBody, const std::map<std::string, std::string>& someHeaders)
	{
		Poco::URI uri(anAddress);
		std::string path(uri.getPathAndQuery());
		if (path.empty()) {
			path = "/";
		}

		Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
		Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, path, Poco::Net::HTTPMessage::HTTP_1_1);
		request.setContentType("application/json");
		for (const auto& header : someHeaders) {
			request.set(header.first, header.second);
		}
		request.setContentLength(aBody.length());

		Poco::Net::HTTPResponse response;
		std::ostream& os = session.sendRequest(request);
		os << aBody;
		std::istream& responseStream = session.receiveResponse(response);
		std::cout << response.getStatus() << " " << response.getReason() << std::endl;
		if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK)
		{
			return "";
		}

		std::stringstream ss;
		Poco::StreamCopier::copyStream(responseStream, ss);
		return ss.str();
	}

	std::string Server::GetMyHttpAdress()
	{
		auto thisHost = Poco::Net::DNS::thisHost();
		std::string myIpv4 = thisHost.addresses().back().toString();
		std::string myAddress = myIpv4 + ":" + std::to_string(mPort);
		return "http://" + myAddress;
	}
}
