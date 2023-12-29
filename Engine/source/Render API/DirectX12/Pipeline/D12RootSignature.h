#pragma once
#include <wrl.h>


namespace Engine {
	class D12RootSignature : public Microsoft::WRL::ComPtr<ID3D12RootSignature>
	{

	public:
		D12RootSignature() = default;
		~D12RootSignature();


		void Initialize(ID3D12Device* pDevice);

		void Release();
	};



}

