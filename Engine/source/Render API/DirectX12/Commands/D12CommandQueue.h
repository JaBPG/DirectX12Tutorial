#pragma once
#include <wrl.h>

namespace Engine {

	//A direct type command queue wrapper
	class YT_API D12CommandQueue : public Microsoft::WRL::ComPtr<ID3D12CommandQueue>
	{
	public:
		D12CommandQueue() = default;
		~D12CommandQueue();

		void Initialize(ID3D12Device* pDevice);

		void M_ExecuteCommandList(ID3D12CommandList* pCommandList);


		void Release();
		void FlushQueue();

		inline ID3D12Fence* GetFence() { return mFence.Get(); }

		inline UINT64 M_GetCurrentFenceValue() { return mCurrentFenceValue; }

	private:
		Microsoft::WRL::ComPtr<ID3D12Fence> mFence;

		UINT64 mCurrentFenceValue = 0;

	};

}


