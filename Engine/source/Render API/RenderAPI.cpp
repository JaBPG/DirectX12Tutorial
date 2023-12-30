#include "pch.h"
#include "RenderAPI.h"

#include <vector>

#include "DirectX12/DXGI/DXGIFactory.h"
#include "DirectX12/DXGI/DXGIAdapter.h"


#include "DirectX12/Debug/D12Debug.h"


#include "../Utilities/Utilities.h"





namespace Engine {
	using namespace Render;


	RenderAPI::~RenderAPI()
	{
		Release();
	}

	void RenderAPI::Initialize(HWND hwnd, const UINT width, const UINT height)
	{
		mWidth = width;
		mHeight = height;

		//this could be disabled during non-debug-builds
		D12Debug::Get().Enable();

		DXGIFactory factory;
		DXGIAdapter adapter = factory.GetAdapter();

		/* CONSOLE OUTPUT FOR DEBUGGING */
		{
			DXGI_ADAPTER_DESC desc;
			adapter->GetDesc(&desc);
			PRINT_W_N("Selected device " << desc.Description);
		}
		/* END DEBUGGING OUTPUT */

		mDevice.Init(adapter.Get());
		mDevice->SetName(L"Main virtual device");

		mCommandQueue.Initialize(mDevice.Get());
		mCommandList.Initialize(mDevice.Get());

		mSwapChain.Initialize(mDevice.Get(), factory.Get(), mCommandQueue.Get(), hwnd, mWidth, mHeight);

		mBufferUploader.Initialize(mDevice.Get(), KBs(64));




		mVertexBuffer.Initialize(mDevice.Get(), KBs(8), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
		mVertexBuffer.Get()->SetName(L"Vertex buffer");


		std::vector<Vertex> vertices;

#define G_BOX_VERTICES 8

		Vertex verticesbox[G_BOX_VERTICES];
		verticesbox[0].position = { -1.000000, 1.000000, 1.000000 };
		verticesbox[1].position = { -1.000000, - 1.000000, 1.000000 };
		verticesbox[2].position = { -1.000000, 1.000000, - 1.00000 };
		verticesbox[3].position = { -1.000000, - 1.000000, - 1.000000 };

		verticesbox[4].position = { 1.000000, 1.000000, 1.000000 };
		verticesbox[4].color = { 1.0f,0.0f,0.0f,1.0f };
		verticesbox[5].position = { 1.000000, - 1.000000, 1.000000 };
		verticesbox[5].color = { 1.0f,0.0f,0.0f,1.0f };
		verticesbox[6].position = { 1.000000, 1.000000, - 1.00000 };
		verticesbox[6].color = { 1.0f,0.0f,0.0f,1.0f };
		verticesbox[7].position = { 1.000000, - 1.000000, - 1.000000 };
		verticesbox[7].color = { 1.0f,0.0f,0.0f,1.0f };
	
		
		mBufferUploader.Upload((D12Resource*)mVertexBuffer.GetAddressOf(), verticesbox, sizeof(Vertex) * G_BOX_VERTICES,
			(D12CommandList*)mCommandList.GetAddressOf(), (D12CommandQueue*)mCommandQueue.GetAddressOf(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

	
		mVBView.BufferLocation = mVertexBuffer.Get()->GetGPUVirtualAddress();
		mVBView.StrideInBytes = sizeof(Vertex);
		mVBView.SizeInBytes = KBs(8);



#define G_INDICES 39

		UINT32 indicies[G_INDICES];
		indicies[0] = 0;
		indicies[1] = 2;
		indicies[2] = 4;

		indicies[3] = 2;
		indicies[4] = 7;
		indicies[5] = 3;

		indicies[6] = 6;
		indicies[7] = 5;
		indicies[8] = 7;

		indicies[9] = 1;
		indicies[10] = 7;
		indicies[11] = 5;

		indicies[12] = 0;
		indicies[13] = 3;
		indicies[14] = 1;

		indicies[15] = 4;
		indicies[16] = 1;
		indicies[17] = 5;

		indicies[18] = 4;
		indicies[19] = 6;
		indicies[20] = 2;

		indicies[21] = 2;
		indicies[22] = 6;
		indicies[23] = 7;

		indicies[24] = 6;
		indicies[25] = 4;
		indicies[26] = 5;

		indicies[27] = 6;
		indicies[28] = 4;
		indicies[29] = 5;

		indicies[30] = 1;
		indicies[31] = 3;
		indicies[32] = 7;

		indicies[33] = 0;
		indicies[34] = 2;
		indicies[35] = 3;

		indicies[36] = 4;
		indicies[37] = 0;
		indicies[38] = 1;
		

		mIndexBuffer.Initialize(mDevice.Get(), KBs(16), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
		mIndexBuffer->SetName(L"Index buffer");

		mBufferUploader.Upload((D12Resource*)mIndexBuffer.GetAddressOf(), indicies, sizeof(UINT32)* G_INDICES,
			(D12CommandList*)mCommandList.GetAddressOf(), (D12CommandQueue*)mCommandQueue.GetAddressOf());

		mIBView.BufferLocation = mIndexBuffer.Get()->GetGPUVirtualAddress();
		mIBView.Format = DXGI_FORMAT_R32_UINT;
		mIBView.SizeInBytes = KBs(16);

	

		mBasePipeline.Initialize(mDevice.Get());

		mDepthBuffer.InitializeAsDepthBuffer(mDevice.Get(), mWidth, mHeight);

		mDepthDescHeap.InitializeDepthHeap(mDevice.Get());

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;


		mDevice->CreateDepthStencilView(mDepthBuffer.Get(), &dsvDesc, mDepthDescHeap.Get()->GetCPUDescriptorHandleForHeapStart());

		mViewport.TopLeftX = 0;
		mViewport.TopLeftY = 0;
		mViewport.Width = mWidth;
		mViewport.Height = mHeight;
		mViewport.MinDepth = 0.0f;
		mViewport.MaxDepth = 1.0f;

		mSRRect.left = 0;
		mSRRect.right = mViewport.Width;
		mSRRect.top = 0;
		mSRRect.bottom = mViewport.Height;



		DirectX::XMMATRIX viewMatrix;
		viewMatrix = DirectX::XMMatrixLookAtLH({ 2.0f, 1.5f,-3.0f,0.0f }, { 0.0f,0.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f,0.0f });

		DirectX::XMMATRIX projectionMatrix;
		projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(1.2217304764f, 16.0f / 9.0f, 1.0f, 50.0f);

		mViewProjectionMatrix = viewMatrix * projectionMatrix;
		
		mCBPassData.Initialize(mDevice.Get(), Utils::CalculateConstantbufferAlignment(sizeof(PassData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);


		/*



		5th Video:
		Add normals to our pipeline (by using some data from a simple blender model)


		6th Video
		- "Material" system (diffuse color)

		7th video
		- A simple "Toon" shader
		- A light source

		8th video 
		- simplify / make the light source controlable from the application side (meaning not from the shaders)
		 		
		
		
		*/


	}

	void RenderAPI::UpdateDraw()
	{
		memcpy(mCBPassData.GetCPUMemory(), &mViewProjectionMatrix, sizeof(PassData));


		
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = mSwapChain.GetCurrentRenderTarget();
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		mCommandList.GFXCmd()->ResourceBarrier(1, &barrier);

		const float clearColor[] = {  0.0f,0.0f,0.0f,1.0f };
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = mSwapChain.GetCurrentRTVHandle();
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = mDepthDescHeap->GetCPUDescriptorHandleForHeapStart();

		mCommandList.GFXCmd()->ClearRenderTargetView(rtvHandle, clearColor, 0, 0);
		mCommandList.GFXCmd()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, 0);
		mCommandList.GFXCmd()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

		mCommandList.GFXCmd()->RSSetViewports(1, &mViewport);
		mCommandList.GFXCmd()->RSSetScissorRects(1, &mSRRect);

		mCommandList.GFXCmd()->SetGraphicsRootSignature(mBasePipeline.GetRS());
		mCommandList.GFXCmd()->SetPipelineState(mBasePipeline.Get());
		mCommandList.GFXCmd()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		mCommandList.GFXCmd()->IASetVertexBuffers(0, 1, &mVBView);
		mCommandList.GFXCmd()->IASetIndexBuffer(&mIBView);

		mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(0, mCBPassData.Get()->GetGPUVirtualAddress());


		//mCommandList.GFXCmd()->DrawInstanced(G_BOX_VERTICES, 1, 0, 0);
		mCommandList.GFXCmd()->DrawIndexedInstanced(G_INDICES, 1, 0, 0, 0);

		barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = mSwapChain.GetCurrentRenderTarget();
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

		mCommandList.GFXCmd()->ResourceBarrier(1, &barrier);


		mCommandList.GFXCmd()->Close();
		mCommandQueue.M_ExecuteCommandList(mCommandList.Get());

		mSwapChain.Present();

		while (mCommandQueue.GetFence()->GetCompletedValue() < mCommandQueue.M_GetCurrentFenceValue()) {

			_mm_pause();
		}

		mCommandList.ResetCommandList();
	}

	void RenderAPI::Release()
	{

		mVertexBuffer.Release();

		mCommandQueue.FlushQueue();
	
		mSwapChain.Release();
		
		mCommandList.Release();

		mCommandQueue.Release();


		if (mDevice.Get()) {

			mDevice.Reset();

		}
	}

}






/*
verticesbox[0].position = { -1.0f,-1.0f,-1.0f }; //0
verticesbox[0].color = { 0.0f,1.0f,0.0f,1.0f };
verticesbox[1].position = { -1.0f,1.0f,-1.0f }; //1
verticesbox[1].color = { 0.0f,1.0f,0.0f,1.0f };
verticesbox[2].position = { 1.0f,-1.0f,-1.0f }; //2
verticesbox[2].color = { 0.0f,1.0f,0.0f,1.0f };

verticesbox[3].position = { -1.0f,1.0f,-1.0f };
verticesbox[3].color = { 0.0f,1.0f,0.0f,1.0f };
verticesbox[4].position = { 1.0f,1.0f,-1.0f };
verticesbox[4].color = { 0.0f,1.0f,0.0f,1.0f };
verticesbox[5].position = { 1.0f,-1.0f,-1.0f };
verticesbox[5].color = { 0.0f,1.0f,0.0f,1.0f };


verticesbox[6].position = { -1.0f,1.0f,1.0f };
verticesbox[6].color = { 1.0f,0.0f,0.0f,1.0f };
verticesbox[7].position = { -1.0f,-1.0f,1.0f };
verticesbox[7].color = { 1.0f,1.0f,0.0f,1.0f };
verticesbox[8].position = { 1.0f,1.0f,1.0f };
verticesbox[8].color = { 1.0f,1.0f,0.0f,1.0f };

verticesbox[9].position = { -1.0f,-1.0f,1.0f };
verticesbox[9].color = { 1.0f,1.0f,0.0f,1.0f };
verticesbox[10].position = { 1.0f,-1.0f,1.0f };
verticesbox[10].color = { 1.0f,1.0f,0.0f,1.0f };
verticesbox[11].position = { 1.0f,1.0f,1.0f };
verticesbox[11].color = { 1.0f,1.0f,0.0f,1.0f };


verticesbox[12].position = { -1.0f,-1.0f,1.0f };
verticesbox[12].color = { 0.0f,0.0f,1.0f,1.0f };
verticesbox[13].position = { -1.0f,1.0f,1.0f };
verticesbox[13].color = { 0.0f,0.0f,1.0f,1.0f };
verticesbox[14].position = { -1.0f,-1.0f,-1.0f };
verticesbox[14].color = { 0.0f,0.0f,1.0f,1.0f };


verticesbox[15].position = { -1.0f,1.0f,1.0f };
verticesbox[15].color = { 0.0f,1.0f,1.0f,1.0f };
verticesbox[16].position = { -1.0f,1.0f,-1.0f };
verticesbox[16].color = { 0.0f,1.0f,1.0f,1.0f };
verticesbox[17].position = { -1.0f,-1.0f,-1.0f };
verticesbox[17].color = { 0.0f,1.0f,1.0f,1.0f };
*/