#pragma once
#include <wrl.h>

namespace Engine {
	class DXGIAdapter : public Microsoft::WRL::ComPtr<IDXGIAdapter>
	{
	public:
		DXGIAdapter() = default;

		DXGIAdapter(IDXGIAdapter* pAdapter);


	};


}
