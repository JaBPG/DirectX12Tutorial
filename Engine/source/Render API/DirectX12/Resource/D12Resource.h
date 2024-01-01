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

		void InitializeAsDepthBuffer(ID3D12Device* pDevice, const unsigned int width, const unsigned int height);

		void Release();
		void* GetCPUMemory();

		/* implement move / copy constructor for object since we store it in vectors and they resize */

	private:
		void* mMemory = nullptr;
	};



}
