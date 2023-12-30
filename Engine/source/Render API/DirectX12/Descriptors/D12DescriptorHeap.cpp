#include "pch.h"
#include "D12DescriptorHeap.h"
namespace Engine {
	D12DescriptorHeap::~D12DescriptorHeap()
	{
		Release();
	}
	void D12DescriptorHeap::InitializeDepthHeap(ID3D12Device* pDevice)
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		desc.NumDescriptors = 1;
		desc.NodeMask = 0;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		YT_EVAL_HR(pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(GetAddressOf())), "Error creating descriptor heap");
	}
	void D12DescriptorHeap::Release()
	{

		if (Get()) {

			Reset();
		}
	}
}