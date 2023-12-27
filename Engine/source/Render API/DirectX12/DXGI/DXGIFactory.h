#pragma once
#include <wrl.h>

#include "DXGIAdapter.h"

namespace Engine {

	class DXGIFactory : public Microsoft::WRL::ComPtr<IDXGIFactory2>
	{
	public:
		DXGIFactory();

		DXGIAdapter GetAdapter();

	};



}


