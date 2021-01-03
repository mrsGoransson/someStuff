#pragma once
#include <string>

namespace emmaChain {
	struct Transaction {
		std::string mSender;
		std::string mRecipient;
		std::string mMessage;
		uint32_t mAmount{};
	};
}
