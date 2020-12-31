
#include "Timer.h"

namespace CU {
	Timer::Timer()
	{
		myStartTime = std::chrono::high_resolution_clock::now();
	}

	void Timer::Update()
	{
		myNewTime = std::chrono::high_resolution_clock::now();
		myTotalTime = myNewTime - myStartTime;
		myDeltaTime = myNewTime - myLastFrame;
		myLastFrame = myNewTime;
	}

	float Timer::GetDeltaTime() const
	{
		return myDeltaTime.count();
	}

	double Timer::GetTotalTime() const
	{
		return myTotalTime.count();
	}
}