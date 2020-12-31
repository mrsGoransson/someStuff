#pragma once
#include "DllApi.h"

#include <chrono>

namespace CU
{
	class StopWatch
	{
	public:
		DLL_API void Start();
		DLL_API void Stop();
		DLL_API float GetTime();

	private:
		std::chrono::high_resolution_clock::time_point myStartTime;
		std::chrono::high_resolution_clock::time_point myEndTime;
		std::chrono::duration<float> myDuration{};
	};
}


