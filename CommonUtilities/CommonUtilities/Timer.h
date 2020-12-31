#pragma once
#include "DllApi.h"

#include <chrono>

namespace CU {
	class Timer {
	public:
		DLL_API Timer();
		Timer(const Timer& anotherTimer) = delete;
		Timer& operator=(const Timer& anotherTimer) = delete;

		DLL_API void Update();
		DLL_API float GetDeltaTime() const;
		DLL_API double GetTotalTime() const;

	private:
		std::chrono::high_resolution_clock::time_point myStartTime;
		std::chrono::high_resolution_clock::time_point myNewTime;
		std::chrono::high_resolution_clock::time_point myLastFrame;

		std::chrono::duration<float> myDeltaTime{};
		std::chrono::duration<double> myTotalTime{};
	};
}