#pragma once
#include <wrl.h>


namespace Engine {

	class DXGIDebug : public Microsoft::WRL::ComPtr<IDXGIDebug>
	{
	public:
		DXGIDebug() = default;


		void Enable();

		void GetLiveObjects();


	private:

		void GetInterface();

	public:
		static inline DXGIDebug& Get() { return mInstance; };


	private:
		static DXGIDebug mInstance;
	};



}

