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
	}

	const Block& Blockchain::NewBlock(int64_t aProof, std::string aPreviousHash)
	{
		mChain.push_back(Block(static_cast<uint32_t>(mChain.size()), aProof, aPreviousHash, mCurrentTransactions));
		mCurrentTransactions.clear();
		return GetLastBlock();
	}

	int Blockchain::NewTransaction(const std::string& aSender, const std::string& aRecipient, const std::string& aMessage, int anAmount)
	{
		mCurrentTransactions.push_back({ aSender, aRecipient, aMessage, anAmount });
		return GetLastBlock().GetIndex() + 1;
	}

	std::string Blockchain::Hash(const Block& aBlock) const
	{
		return aBlock.CalculateHash();
	}

	int64_t Blockchain::ProofOfWork(int64_t aLastProof)
	{
		int64_t proof = 0;
		while (!ValidProof(aLastProof, proof, mDifficulty))
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

	bool Blockchain::ValidChain(const std::vector<Block>& aChain)
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
			if (!ValidProof(lastBlock->GetProof(), block.GetProof(), mDifficulty))
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
				const auto& chainJson = jsonDocument["chain"].GetArray();
				std::vector<Block> neighboursChain;
				for (const auto& block : chainJson)
				{
					std::vector<Transaction> neighboursBlockTransactions;
					if (block.HasMember("transactions") && block["transactions"].IsArray())
					{
						const auto& transactionsJson = block["transactions"].GetArray();
						for (const auto& transaction : transactionsJson)
						{
							neighboursBlockTransactions.push_back(Transaction{ transaction["sender"].GetString(),
																				transaction["recipient"].GetString(),
																				transaction["message"].GetString(),
																				transaction["amount"].GetInt() });
						}
					}
					neighboursChain.push_back(Block(block["index"].GetUint(), block["proof"].GetInt64(), block["previous_hash"].GetString(), 
												neighboursBlockTransactions, block["timestamp"].GetInt64()));
				}

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

	bool Blockchain::ValidProof(int64_t aLastProof, int64_t aProof, uint32_t aDifficulty) const
	{
		std::string shoulStartWith;
		shoulStartWith.resize(aDifficulty, '0');

		std::string guess = std::to_string(aLastProof) + std::to_string(aProof);
		std::string hash = CU::sha256(guess);
		return hash.substr(0, aDifficulty) == shoulStartWith;
	}
}