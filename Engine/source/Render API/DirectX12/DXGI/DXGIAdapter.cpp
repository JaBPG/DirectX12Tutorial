#include "pch.h"
#include "DXGIAdapter.h"


namespace Engine {
	DXGIAdapter::DXGIAdapter(IDXGIAdapter* pAdapter) : Microsoft::WRL::ComPtr<IDXGIAdapter>(pAdapter)
	{

	}

}

