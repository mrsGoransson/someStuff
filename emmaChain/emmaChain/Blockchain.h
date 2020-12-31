#pragma once
#include "Block.h"

#include <set>

namespace emmaChain {
	class Server;

	class Blockchain {
	public:
		Blockchain(Server& aServer);

		const Block& NewBlock(int64_t aProof, std::string aPreviousHash);
		int NewTransaction(const std::string& aSender, const std::string& aRecipient, const std::string& aMessage, int anAmount);
		std::string Hash(const Block& aBlock) const;
		int64_t ProofOfWork(int64_t aLastProof);
		void RegisterNode(const std::string& anAddress);
		bool ValidChain(const std::vector<Block>& aChain);
		bool ResolveConflicts();

		const std::vector<Block>& GetChain() const;
		const Block& GetLastBlock() const;
		const std::set<std::string>& GetNodes() const;

	private:
		void CreateGenesisBlock();
		bool ValidProof(int64_t aLastProof, int64_t aProof, uint32_t aDifficulty) const;

		uint32_t mDifficulty;
		std::vector<Block> mChain;
		std::vector<Transaction> mCurrentTransactions;
		std::set<std::string> mNodes;
		Server& mServer;
	};
}