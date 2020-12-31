#pragma once
#include "DllApi.h"

#include <random>

namespace CU
{
	namespace Random
	{
		float GetRandomFloat(float aLowest, float aHighest)
		{
			static std::random_device random;
			std::mt19937 changeRandom(random());
			std::uniform_real_distribution<float> randomLimits(aLowest, aHighest);

			float randomInt = randomLimits(random);
			return randomInt;
		}

		int GetRandomInt(int aLowestNumber, int aHighestNumber)
		{
			static std::random_device random;
			std::mt19937 changeRandom(random());
			std::uniform_int_distribution<unsigned> randomLimits(aLowestNumber, aHighestNumber);

			int randomInt = randomLimits(random);
			return randomInt;
		}
	}
}


