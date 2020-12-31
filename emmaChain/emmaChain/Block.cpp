#include "Block.h"

#include <CommonUtilities/sha256/sha256.h>

#include <sstream>

namespace emmaChain {
	Block::Block(uint32_t anIndexIn, int64_t aProof, const std::string& aPreviousHash, const std::vector<Transaction>& someTransactions)
		: mTransactions(someTransactions)
		, mPreviousHash(aPreviousHash)
		, mProof(aProof)
		, mIndex(anIndexIn)
	{
		mTimestamp = time(nullptr);
	}

	Block::Block(uint32_t anIndexIn, int64_t aProof, const std::string& aPreviousHash, const std::vector<Transaction>& someTransactions, int64_t aTimestamp)
		: mTransactions(someTransactions)
		, mPreviousHash(aPreviousHash)
		, mTimestamp(aTimestamp)
		, mProof(aProof)
		, mIndex(anIndexIn)
	{
	}

	const std::vector<Transaction>& Block::GetTransactions() const
	{
		return mTransactions;
	}

	const std::string& Block::GetPreviousHash() const
	{
		return mPreviousHash;
	}

	const time_t Block::GetTimestamp() const
	{
		return mTimestamp;
	}

	int64_t Block::GetProof() const
	{
		return mProof;
	}

	int Block::GetIndex() const
	{
		return mIndex;
	}

	std::string Block::CalculateHash() const
	{
		std::stringstream ss;
		ss << mIndex << mTimestamp << mProof << mPreviousHash;
		return CU::sha256(ss.str());
	}
}