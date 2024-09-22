#pragma once
#include <wrl.h>

namespace Engine {
	class D12Resource : public Microsoft::WRL::ComPtr<ID3D12Resource>
	{

	public:
		D12Resource() = default;
		~D12Resource();

		D12Resource(const D12Resource& other) = delete;
		D12Resource& operator=(const D12Resource& other) = delete;

		D12Resource(D12Resource&& other) noexcept;
		D12Resource& operator=(D12Resource&& other) noexcept;


		void Initialize(ID3D12Device* pDevice, const unsigned int numBytes, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState);

		void InitializeAsDepthBuffer(ID3D12Device* pDevice, const unsigned int width, const unsigned int height, const DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT);

		void InitializeAsTexture(ID3D12Device* pDevice, const unsigned int width, const unsigned int height, const DXGI_FORMAT format);

		void Release();
		void* GetCPUMemory();

		D3D12_SHADER_RESOURCE_VIEW_DESC& GetSRV() { return mView.srv; }

	

	private:

		void CreateSRV();

		union View
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC srv;
		};

		View mView;

		void* mMemory = nullptr;
	};



}
