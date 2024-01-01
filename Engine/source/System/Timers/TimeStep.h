#pragma once
#include <chrono>

namespace Engine {

	namespace Timer {

		class YT_API TimeStep
		{
		public:

			TimeStep();

			float Tick();


		private:

			std::chrono::steady_clock::time_point mLastTime;
		};
	}

}





