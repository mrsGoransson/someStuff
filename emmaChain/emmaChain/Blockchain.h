#pragma once
#include "Block.h"

#include <rapidjson/document.h>

#include <set>


namespace emmaChain {
	class Server;

	class Blockchain {
	public:
		Blockchain(Server& aServer);

		static std::vector<Block> ConstructChainFromJson(rapidjson::Document& aJsonDocument);

		bool Mine(const std::string& aNodeIdentifier);
		const Block& AddNewBlock(int64_t aProof, std::string aPreviousHash);
		int NewTransaction(const std::string& aSender, const std::string& aRecipient, const std::string& aMessage, uint32_t anAmount);
		std::string Hash(const Block& aBlock) const;
		int64_t ProofOfWork(int64_t aLastProof);
		void RegisterNode(const std::string& anAddress);
		bool ValidChain(const std::vector<Block>& aChain) const;
		bool ResolveConflicts();
		void OverwriteLocalChain(const std::vector<Block>& aNewChain);
		bool AddBlock(const Block& aNewBlock);

		const std::vector<Block>& GetChain() const;
		const Block& GetLastBlock() const;
		const std::set<std::string>& GetNodes() const;
		const std::vector<Transaction>& GetPendingTransactions() const;

	private:
		void CreateGenesisBlock();
		bool ValidProof(int64_t aLastProof, int64_t aProof) const;

		uint32_t mDifficulty;
		std::vector<Block> mChain;
		std::vector<Transaction> mPendingTransactions;
		std::set<std::string> mNodes;
		Server& mServer;
	};
}