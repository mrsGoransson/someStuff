#pragma once
#include "Transaction.h"

#include <cstdint>
#include <string>
#include <vector>

namespace emmaChain {
	class Block {
	public:
		Block(uint32_t anIndexIn, int64_t aProof, const std::string& aPreviousHash, const std::vector<Transaction>& someTransactions);
		Block(uint32_t anIndexIn, int64_t aProof, const std::string& aPreviousHash, const std::vector<Transaction>& someTransactions, int64_t aTimestamp);

		std::string CalculateHash() const;

		const std::vector<Transaction>& GetTransactions() const;
		const std::string& GetPreviousHash() const;
		const time_t GetTimestamp() const;
		int64_t GetProof() const;
		int GetIndex() const;

	private:
		std::vector<Transaction> mTransactions;
		std::string mPreviousHash;
		time_t mTimestamp{};
		int64_t mProof{};
		uint32_t mIndex{};
	};
}