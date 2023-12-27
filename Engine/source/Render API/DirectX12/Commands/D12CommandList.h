#pragma once
#include <wrl.h>

namespace Engine {
	class YT_API D12CommandList : public Microsoft::WRL::ComPtr<ID3D12CommandList>
	{
	public:
		D12CommandList() = default;
		~D12CommandList();

		void Initialize(ID3D12Device* pDevice);

		void ResetCommandList();

		inline ID3D12GraphicsCommandList* GFXCmd() { return (ID3D12GraphicsCommandList*)Get(); }

		void Release();

	private:

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mAllocator;

	};


}

