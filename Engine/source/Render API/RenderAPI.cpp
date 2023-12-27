#include "pch.h"
#include "RenderAPI.h"


#include "DirectX12/DXGI/DXGIFactory.h"
#include "DirectX12/DXGI/DXGIAdapter.h"


#include "DirectX12/Debug/D12Debug.h"



namespace Engine {



	RenderAPI::~RenderAPI()
	{
		if (mDevice.Get()) {

			mDevice.Reset();

		}

	}

	void RenderAPI::Initialize(HWND hwnd)
	{
		//this could be disabled during non-debug-builds
		D12Debug::Get().Enable();

		DXGIFactory factory;
		DXGIAdapter adapter = factory.GetAdapter();

		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);


		PRINT_W_N("Selected device " << desc.Description);

		mDevice.Init(adapter.Get());

		mDevice->SetName(L"Main virtual device");

	}

}