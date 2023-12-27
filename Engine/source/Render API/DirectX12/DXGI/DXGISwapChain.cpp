#include "pch.h"
#include "DXGISwapChain.h"


namespace Engine {
	DXGISwapChain::~DXGISwapChain()
	{
		Release();

	}
	void DXGISwapChain::Initialize(ID3D12Device* pDevice, IDXGIFactory2* pFactory, ID3D12CommandQueue* pCommandQueue, const HWND hwnd, const UINT width, const UINT height)
	{

		mWidth = width;
		mHeight = height;

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.NumDescriptors = G_MAX_SWAPCHAIN_BUFFERS; 
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = 0;

		YT_EVAL_HR(pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(mRTVHeap.GetAddressOf())), "Error creating the RTV heap");
		mHeapIncrement = pDevice->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);

		DXGI_SWAP_CHAIN_DESC1 description = {};
		description.Width = width;
		description.Height = height;
		description.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		description.Stereo = false;
		description.SampleDesc = { 1,0 };
		description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		description.BufferCount = G_MAX_SWAPCHAIN_BUFFERS;
		description.Scaling = DXGI_SCALING_NONE;
		description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		description.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		description.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; 


		YT_EVAL_HR(pFactory->CreateSwapChainForHwnd(pCommandQueue, hwnd, &description, nullptr, nullptr, GetAddressOf()), "Error creating swapchain");

		mBufferCount = G_MAX_SWAPCHAIN_BUFFERS;

		mDevice = pDevice;

		CreateBuffers();

	}
	void DXGISwapChain::CreateBuffers()
	{
		if (!Get()) {
			PRINT_N("Error: Tried to create buffers for uninitialized SwapChain");
			return;
		}

		for (int i = 0; i < mBufferCount; i++) {
			mRenderTargets[i].Reset();

			YT_EVAL_HR(Get()->GetBuffer(i, IID_PPV_ARGS(mRenderTargets[i].GetAddressOf())), "Error creating buffer for swapchain idx: " << i);

			D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = mRTVHeap->GetCPUDescriptorHandleForHeapStart();
			cpuHandle.ptr += (size_t)mHeapIncrement * i;

			mDevice->CreateRenderTargetView(mRenderTargets[i].Get(), 0, cpuHandle);

		}

	}
	void DXGISwapChain::DropBuffers()
	{
		for (int i = 0; i < mBufferCount; i++) {
			mRenderTargets[i].Reset();

		}
	}
	void DXGISwapChain::Present()
	{
		Get()->Present(0, 0); //DXGI_PRESENT_ALLOW_TEARING
	
		mCurrentBuffer = (mCurrentBuffer + 1) % mBufferCount;
	}
	D3D12_CPU_DESCRIPTOR_HANDLE DXGISwapChain::GetCurrentRTVHandle()
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = mRTVHeap->GetCPUDescriptorHandleForHeapStart();
		cpuHandle.ptr += (size_t)mCurrentBuffer * mHeapIncrement;

		return cpuHandle;
	}
	void DXGISwapChain::Release()
	{

		if (Get()) {

			Reset();
			DropBuffers();
			mRTVHeap.Reset();
		}

		
	}
}
