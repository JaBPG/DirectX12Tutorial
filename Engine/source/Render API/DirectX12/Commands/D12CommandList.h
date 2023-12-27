#pragma once
#include <wrl.h>

namespace Engine {
	class D12CommandList : public Microsoft::WRL::ComPtr<ID3D12CommandList>
	{
	public:
		D12CommandList() = default;
		~D12CommandList();

		void Initialize(ID3D12Device* pDevice);

		void Release();

	private:

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mAllocator;

	};


}

