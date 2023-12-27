#pragma once
#include "EngineMin.h"
#include <Windows.h>


#include "DirectX12/Base/D12Device.h"
#include "DirectX12/Commands/D12CommandQueue.h"
#include "DirectX12/Commands/D12CommandList.h"

namespace Engine {

	class YT_API RenderAPI
	{

	public: 
		RenderAPI() = default;
		~RenderAPI();

		void Initialize(HWND hwnd);

		void Release();

	private:

		D12Device mDevice;
		D12CommandQueue mCommandQueue;
		D12CommandList mCommandList;


	};
}


