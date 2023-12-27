#include "pch.h"
#include "D12Device.h"

namespace Engine {




	D12Device::D12Device(IDXGIAdapter* pAdapter)
	{
		Init(pAdapter);
	}

	void D12Device::Init(IDXGIAdapter* pAdapter)
	{
		YT_EVAL_HR(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(GetAddressOf())), "Error creating D12 device");
	}

}
