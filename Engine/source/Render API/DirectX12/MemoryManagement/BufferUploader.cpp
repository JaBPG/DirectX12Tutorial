#include "pch.h"
#include "BufferUploader.h"


namespace Engine {



	BufferUploader::~BufferUploader()
	{
		Release();
	}

	void BufferUploader::Initialize(ID3D12Device* pDevice, const UINT memoryallocation)
	{

		mInternalBuffer.Initialize(pDevice, memoryallocation, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);

		mInternalMemory = mInternalBuffer.GetCPUMemory();
		mMaxSize = memoryallocation;

	}

	void BufferUploader::Upload(D12Resource* pDestination, void* pSourceData, const UINT size, D12CommandList* pCommandList, D12CommandQueue* pCommandQueue, D3D12_RESOURCE_STATES customState)
	{
		YT_ASSERT(size < mMaxSize);

		memcpy(mInternalMemory, pSourceData, size);

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = pDestination->Get();
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;

		pCommandList->GFXCmd()->ResourceBarrier(1, &barrier);
		pCommandList->GFXCmd()->CopyBufferRegion(pDestination->Get(), 0, mInternalBuffer.Get(), 0, size);


		barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = pDestination->Get();
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = (customState == D3D12_RESOURCE_STATE_COMMON ? D3D12_RESOURCE_STATE_COMMON : customState);

		pCommandList->GFXCmd()->ResourceBarrier(1, &barrier);

		pCommandList->GFXCmd()->Close();

		pCommandQueue->M_ExecuteCommandList(pCommandList->Get());

		while (pCommandQueue->GetFence()->GetCompletedValue() < pCommandQueue->M_GetCurrentFenceValue()) {

			_mm_pause();
		}


		pCommandList->ResetCommandList();
	}

	void BufferUploader::Release()
	{
		if (mInternalMemory) {
			mInternalMemory = nullptr;
		}
		mInternalBuffer.Release();
	}

}
