#include "Blockchain.h"

#include "Server.h"

#include <CommonUtilities/sha256/sha256.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/URI.h>
#include <rapidjson/document.h>

#include <iostream>
#include <sstream>


namespace emmaChain {
	Blockchain::Blockchain(Server& aServer)
		: mServer(aServer)
	{
		mDifficulty = 3;
		CreateGenesisBlock();
		RegisterNode(mServer.GetMyHttpAdress());
	}

	std::vector<Block> Blockchain::ConstructChainFromJson(rapidjson::Document& aJsonDocument)
	{
		std::vector<Block> result;
		auto chainJson = aJsonDocument["chain"].GetArray();
		std::vector<Block> neighboursChain;
		for (const auto& block : chainJson)
		{
			std::vector<Transaction> transactions;
			if (block.HasMember("transactions") && block["transactions"].IsArray())
			{
				auto transactionsJson = block["transactions"].GetArray();
				for (const auto& transaction : transactionsJson)
				{
					transactions.push_back(Transaction{ transaction["sender"].GetString(),
																		transaction["recipient"].GetString(),
																		transaction["message"].GetString(),
																		transaction["amount"].GetUint() });
				}
			}
			result.push_back(Block(block["index"].GetUint(), block["proof"].GetInt64(), block["previous_hash"].GetString(),
				transactions, block["timestamp"].GetInt64()));
		}

		return result;
	}

	bool Blockchain::Mine(const std::string& aNodeIdentifier)
	{
		if (mPendingTransactions.empty())
		{
			return false;
		}

		const auto& lastBlock = GetLastBlock();
		int64_t lastProof = lastBlock.GetProof();
		int64_t proof = ProofOfWork(lastProof);
		NewTransaction("0", aNodeIdentifier, "Mining reward", 1);
		std::string previousHash = Hash(lastBlock);
		AddNewBlock(proof, previousHash);
		return true;
	}

	const Block& Blockchain::AddNewBlock(int64_t aProof, std::string aPreviousHash)
	{
		mChain.push_back(Block(static_cast<uint32_t>(mChain.size()), aProof, aPreviousHash, mPendingTransactions));
		mPendingTransactions.clear();
		return GetLastBlock();
	}

	int Blockchain::NewTransaction(const std::string& aSender, const std::string& aRecipient, const std::string& aMessage, uint32_t anAmount)
	{
		mPendingTransactions.push_back({ aSender, aRecipient, aMessage, anAmount });
		return GetLastBlock().GetIndex() + 1;
	}

	std::string Blockchain::Hash(const Block& aBlock) const
	{
		return aBlock.CalculateHash();
	}

	int64_t Blockchain::ProofOfWork(int64_t aLastProof)
	{
		int64_t proof = 0;
		while (!ValidProof(aLastProof, proof))
		{
			proof++;
		}
		return proof;
	}

	void Blockchain::RegisterNode(const std::string& anAddress)
	{
		Poco::URI uri(anAddress);
		std::string authority(uri.getAuthority());
		mNodes.insert(authority);
	}

	bool Blockchain::ValidChain(const std::vector<Block>& aChain) const
	{
		const Block* lastBlock = &aChain.front();
		size_t currentIndex = 1;

		while (currentIndex < aChain.size())
		{
			auto& block = aChain.at(currentIndex);
			if (block.GetPreviousHash() != Hash(*lastBlock))
			{
				return false;
			}
			if (!ValidProof(lastBlock->GetProof(), block.GetProof()))
			{
				return false;
			}
			lastBlock = &block;
			currentIndex++;
		}

		return true;
	}

	bool Blockchain::ResolveConflicts()
	{
		unsigned int maxLength = static_cast<unsigned int>(mChain.size());
		std::vector<Block> newChain;

		for (auto& neighbour : mNodes)
		{
			const auto& response = mServer.HttpGet("http://" + neighbour + "/chain");
			if (!response.empty())
			{
				rapidjson::Document jsonDocument;
				jsonDocument.Parse(response.c_str());
				unsigned int length = jsonDocument["length"].GetUint();
				std::vector<Block> neighboursChain = ConstructChainFromJson(jsonDocument);
				if (length > maxLength and ValidChain(neighboursChain))
				{
					maxLength = length;
					newChain = neighboursChain;
				}
			}
		}
		if (!newChain.empty())
		{
			mChain = newChain;
			return true;
		}
		return false;
	}

	void Blockchain::OverwriteLocalChain(const std::vector<Block>& aNewChain)
	{
		mChain = aNewChain;
	}

	bool Blockchain::AddBlock(const Block& aNewBlock)
	{
		const auto& lastBlock = GetLastBlock();
		const auto& previousHash = Hash(lastBlock);
		if (aNewBlock.GetPreviousHash() != previousHash)
		{
			return false;
		}

		if (!ValidProof(lastBlock.GetProof(), aNewBlock.GetProof()))
		{
			return false;
		}

		mChain.push_back(aNewBlock);
		return true;
	}

	const std::vector<Block>& Blockchain::GetChain() const
	{
		return mChain;
	}

	void Blockchain::CreateGenesisBlock()
	{
		mChain.push_back(Block(0, 100, "1", std::vector<Transaction>()));
	}

	const Block& Blockchain::GetLastBlock() const
	{
		return mChain.back();
	}

	const std::set<std::string>& Blockchain::GetNodes() const
	{
		return mNodes;
	}

	const std::vector<Transaction>& Blockchain::GetPendingTransactions() const
	{
		return mPendingTransactions;
	}

	bool Blockchain::ValidProof(int64_t aLastProof, int64_t aProof) const
	{
		std::string shoulStartWith;
		shoulStartWith.resize(mDifficulty, '0');

		std::string guess = std::to_string(aLastProof) + std::to_string(aProof);
		std::string hash = CU::sha256(guess);
		return hash.substr(0, mDifficulty) == shoulStartWith;
	}
}