#include "pch.h"
#include "DXGIDebug.h"


namespace Engine {

	DXGIDebug DXGIDebug::mInstance;

	void DXGIDebug::Enable()
	{
		GetInterface();

	}

	void DXGIDebug::GetLiveObjects()
	{
		GetInterface();
		Get()->ReportLiveObjects(DXGI_DEBUG_ALL, (DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
	}

	void DXGIDebug::GetInterface()
	{

		if (!Get()) {

			YT_EVAL_HR(DXGIGetDebugInterface1(0,IID_PPV_ARGS(GetAddressOf())), "Error creating the DXGI Debug interface");

		}
	}

}