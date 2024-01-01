#include "pch.h"
#include "TimeStep.h"


namespace Engine {

	namespace Timer {
		TimeStep::TimeStep()
		{
			mLastTime = std::chrono::steady_clock::now();
		}
		float TimeStep::Tick()
		{
			const std::chrono::steady_clock::time_point oldTime = mLastTime;
			mLastTime = std::chrono::steady_clock::now();

			const std::chrono::duration<float> frameTime = mLastTime - oldTime;

			return frameTime.count();

		}
	}
}