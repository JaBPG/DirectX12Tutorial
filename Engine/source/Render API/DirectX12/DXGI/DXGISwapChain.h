#pragma once
#include "EngineSettings.h"
#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>

namespace Engine {
	class YT_API DXGISwapChain : public Microsoft::WRL::ComPtr<IDXGISwapChain1>
	{
	public:
		DXGISwapChain() = default;
		~DXGISwapChain();

		void Initialize(ID3D12Device* pDevice, IDXGIFactory2* pFactory, ID3D12CommandQueue* pCommandQueue, const HWND hwnd, const UINT width, const UINT height);
		void CreateBuffers();
		void DropBuffers();

		void Present();

		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTVHandle();
		inline ID3D12Resource* GetCurrentRenderTarget() { return mRenderTargets[mCurrentBuffer].Get(); }

		void Release();

	private: 
		ID3D12Device* mDevice = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> mRenderTargets[G_MAX_SWAPCHAIN_BUFFERS];
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRTVHeap;

		UINT mBufferCount = 0;
		UINT mCurrentBuffer = 0;

		UINT mWidth = 0, mHeight = 0;
		UINT mHeapIncrement = 0;

	};


}

