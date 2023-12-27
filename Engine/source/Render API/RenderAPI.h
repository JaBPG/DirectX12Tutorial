#pragma once
#include "EngineMin.h"
#include <Windows.h>

#include "DirectX12/DXGI/DXGISwapChain.h"

#include "DirectX12/Base/D12Device.h"
#include "DirectX12/Commands/D12CommandQueue.h"
#include "DirectX12/Commands/D12CommandList.h"



namespace Engine {

	class YT_API RenderAPI
	{

	public: 
		RenderAPI() = default;
		~RenderAPI();

		void Initialize(HWND hwnd, const UINT width, const UINT height);

		void UpdateDraw();

		void Release();

	private:

		D12Device mDevice;
		D12CommandQueue mCommandQueue;
		D12CommandList mCommandList;

		DXGISwapChain mSwapChain;


	private: 
		UINT mWidth = 0;
		UINT mHeight = 0;


	private: //temp testvars
		UINT mCurentidx = 0;
	};
}


