#include "pch.h"
#include "RenderAPI.h"


#include "DirectX12/DXGI/DXGIFactory.h"
#include "DirectX12/DXGI/DXGIAdapter.h"


#include "DirectX12/Debug/D12Debug.h"



namespace Engine {



	RenderAPI::~RenderAPI()
	{
		Release();
	}

	void RenderAPI::Initialize(HWND hwnd, const UINT width, const UINT height)
	{
		mWidth = width;
		mHeight = height;


		//this could be disabled during non-debug-builds
		D12Debug::Get().Enable();

		DXGIFactory factory;
		DXGIAdapter adapter = factory.GetAdapter();

		/* CONSOLE OUTPUT FOR DEBUGGING */
		{
			DXGI_ADAPTER_DESC desc;
			adapter->GetDesc(&desc);
			PRINT_W_N("Selected device " << desc.Description);
		}
		/* END DEBUGGING OUTPUT */

		mDevice.Init(adapter.Get());
		mDevice->SetName(L"Main virtual device");

		mCommandQueue.Initialize(mDevice.Get());
		mCommandList.Initialize(mDevice.Get());


		mSwapChain.Initialize(mDevice.Get(), factory.Get(), mCommandQueue.Get(), hwnd, mWidth, mHeight);




	}

	void RenderAPI::UpdateDraw()
	{
		

	}

	void RenderAPI::Release()
	{

		mSwapChain.Release();

		mCommandList.Release();

		mCommandQueue.Release();


		if (mDevice.Get()) {

			mDevice.Reset();

		}
	}

}