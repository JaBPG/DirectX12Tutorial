#include "pch.h"
#include "D12CommandList.h"


namespace Engine {
	D12CommandList::~D12CommandList()
	{

		Release();

	}
	void D12CommandList::Initialize(ID3D12Device* pDevice)
	{
		//Direct Command List


		YT_EVAL_HR(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mAllocator.GetAddressOf())), "Error creating command allocator");

		YT_EVAL_HR(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mAllocator.Get(), nullptr, IID_PPV_ARGS(GetAddressOf())), "Error creating the commandlist");

	}
	void D12CommandList::Release()
	{
		if (Get()) {

			Reset();
		}


		if (mAllocator.Get()) {
			mAllocator.Reset();

		}
	}
}

