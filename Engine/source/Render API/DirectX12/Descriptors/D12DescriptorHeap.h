#pragma once
#include <wrl.h>
namespace Engine {
	class D12DescriptorHeap : public Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>
	{
	public:

		D12DescriptorHeap() = default;
		~D12DescriptorHeap();

		void InitializeDepthHeap(ID3D12Device* pDevice);


		void Release();
	};

}


