#pragma once
#include "EngineMin.h"

#include <Windows.h>

#include "Render API/RenderAPI.h"

#include "System/Timers/TimeStep.h"




namespace Engine {
	class YT_API Application
	{
	public:
		Application() = default;

		bool Initialize();

		void OnCreate(HWND hwnd);
		

		void Update();


		void OnDestroy();

		inline bool IsRunning() { return mIsRunning; }


	private: //subsystems:

		RenderAPI mRenderer;
		Timer::TimeStep mTimeStepSystem;


	private: //variables:
		HWND mWindowHandle = nullptr;
		bool mIsRunning = false;


	private:
		UINT mWidth = 1280;
		UINT mHeight = 720;
	};
}


