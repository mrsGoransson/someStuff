#pragma once
#include "StopWatch.h"

namespace CU {
	void StopWatch::Start()
	{
		myStartTime = std::chrono::high_resolution_clock::now();
	}

	void StopWatch::Stop()
	{
		myEndTime = std::chrono::high_resolution_clock::now();
		myDuration = myEndTime - myStartTime;
	}

	float StopWatch::GetTime()
	{
		return myDuration.count();
	}
}
