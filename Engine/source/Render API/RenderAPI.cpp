#include "pch.h"
#include "RenderAPI.h"

#include <string>

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

#define G_BOX_VERTICES 24

		Vertex verticesbox[G_BOX_VERTICES];
		verticesbox[0].position = { -1.000000, 1.000000, 1.000000 };
		verticesbox[0].normal = { 0.0000, 1.0000, 0.0000 };

		verticesbox[1].position = { -1.000000, 1.000000, - 1.000000 };
		verticesbox[1].normal = { 0.0000, 1.0000, 0.0000 };

		verticesbox[2].position = { 1.000000, 1.000000, 1.000000 };
		verticesbox[2].normal = { 0.0000, 1.0000, 0.0000 };

		verticesbox[3].position = { -1.000000, - 1.000000, - 1.000000 };
		verticesbox[3].normal = { 0.0000, 0.0000, - 1.0000 };

		verticesbox[4].position = { 1.000000, - 1.000000, - 1.000000 };
		verticesbox[4].normal = { 0.0000, 0.0000, - 1.0000 };

		verticesbox[5].position = { -1.000000, 1.000000, - 1.000000 };
		verticesbox[5].normal = { 0.0000, 0.0000, - 1.0000 };
	
		verticesbox[6].position = { 1.000000, - 1.000000, - 1.000000 };
		verticesbox[6].normal = { 1.0000, 0.0000, 0.0000 };

		verticesbox[7].position = { 1.000000, - 1.000000, 1.00000 };
		verticesbox[7].normal = { 1.0000, 0.0000 ,0.0000 };

		verticesbox[8].position = { 1.000000, 1.000000, - 1.000000 };
		verticesbox[8].normal = { 1.0000, 0.0000, 0.0000 };

		verticesbox[9].position = { 1.000000, - 1.000000, 1.000000 };
		verticesbox[9].normal = { 0.0000, - 1.0000, 0.0000 };

		verticesbox[10].position = { 1.000000, - 1.000000, - 1.000000 };
		verticesbox[10].normal = { 0.0000, - 1.0000, 0.0000 };

		verticesbox[11].position = { -1.000000, - 1.000000, 1.000000 };
		verticesbox[11].normal = { 0.0000, - 1.0000, 0.0000 };

		verticesbox[12].position = { -1.000000, - 1.000000, 1.000000 };
		verticesbox[12].normal = { -1.0000, 0.0000 ,0.0000 };

		verticesbox[13].position = { -1.000000, - 1.000000, - 1.000000 };
		verticesbox[13].normal = { -1.0000, 0.0000, 0.0000 };

		verticesbox[14].position = { -1.000000, 1.000000, 1.000000 };
		verticesbox[14].normal = { -1.0000, 0.0000, 0.0000 };

		verticesbox[15].position = { 1.000000, - 1.000000, 1.00000 };
		verticesbox[15].normal = { 0.0000, 0.0000 ,1.0000 };

		verticesbox[16].position = { -1.000000, - 1.000000, 1.000000 };
		verticesbox[16].normal = { 0.0000, 0.0000, 1.0000 };

		verticesbox[17].position = { 1.000000, 1.000000, 1.000000 };
		verticesbox[17].normal = { 0.0000, 0.0000, 1.0000 };

		//new vertices:

		verticesbox[18].position = { 1.000000, 1.000000, - 1.000000 };
		verticesbox[18].normal = { 0.0000, 1.0000, 0.0000 };

		verticesbox[19].position = { 1.000000, 1.000000, -1.000000 };
		verticesbox[19].normal = { 0.0000, 0.0000 ,- 1.0000 };

		verticesbox[20].position = { 1.000000, 1.000000, 1.000000 };
		verticesbox[20].normal = { 1.0000, 0.0000, 0.0000 };

		verticesbox[21].position = { -1.000000, - 1.000000, - 1.000000 };
		verticesbox[21].normal = { 0.0000 ,- 1.0000, 0.0000 };

		verticesbox[22].position = { -1.000000, 1.000000, - 1.000000 };
		verticesbox[22].normal = { -1.0000, 0.0000, 0.0000 };

		verticesbox[23].position = { -1.000000, 1.000000, 1.000000 };
		verticesbox[23].normal = { 0.0000, 0.0000, 1.0000 };

		
		mBufferUploader.Upload((D12Resource*)mVertexBuffer.GetAddressOf(), verticesbox, sizeof(Vertex) * G_BOX_VERTICES,
			(D12CommandList*)mCommandList.GetAddressOf(), (D12CommandQueue*)mCommandQueue.GetAddressOf(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

	
		mVBView.BufferLocation = mVertexBuffer.Get()->GetGPUVirtualAddress();
		mVBView.StrideInBytes = sizeof(Vertex);
		mVBView.SizeInBytes = KBs(8);



#define G_INDICES 36

		UINT32 indicies[G_INDICES];
		//ib: 0,1,2, 3,4,5 6,7,8 9,10,11 12,13,14 15,16,17
		
		indicies[0] = 0;
		indicies[1] = 1;
		indicies[2] = 2;

		indicies[3] = 3;
		indicies[4] = 4;
		indicies[5] = 5;

		indicies[6] = 6;
		indicies[7] = 7;
		indicies[8] = 8;

		indicies[9] = 9;
		indicies[10] = 10;
		indicies[11] = 11;

		indicies[12] = 12;
		indicies[13] = 13;
		indicies[14] = 14;

		indicies[15] = 15;
		indicies[16] = 16;
		indicies[17] = 17;
		
		indicies[18] = 1;
		indicies[19] = 18;
		indicies[20] = 2;

		indicies[21] = 4;
		indicies[22] = 19;
		indicies[23] = 5;

		indicies[24] = 7;
		indicies[25] = 20;
		indicies[26] = 8;

		indicies[27] = 10;
		indicies[28] = 21;
		indicies[29] = 11;

		indicies[30] = 13;
		indicies[31] = 22;
		indicies[32] = 14;

		indicies[33] = 16;
		indicies[34] = 23;
		indicies[35] = 17;

		mIndexBuffer.Initialize(mDevice.Get(), KBs(16), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
		mIndexBuffer->SetName(L"Index buffer");

		mBufferUploader.Upload((D12Resource*)mIndexBuffer.GetAddressOf(), indicies, sizeof(UINT32)* G_INDICES,
			(D12CommandList*)mCommandList.GetAddressOf(), (D12CommandQueue*)mCommandQueue.GetAddressOf());

		mIBView.BufferLocation = mIndexBuffer.Get()->GetGPUVirtualAddress();
		mIBView.Format = DXGI_FORMAT_R32_UINT;
		mIBView.SizeInBytes = KBs(16);

	

		mBasePipeline.Initialize(mDevice.Get());
		mPlanarShadowPipeline.InitializeAsTransparent(mDevice.Get());

		mDepthBuffer.InitializeAsDepthBuffer(mDevice.Get(), mWidth, mHeight);

		mDepthDescHeap.InitializeDepthHeap(mDevice.Get());

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
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
		viewMatrix = DirectX::XMMatrixLookAtLH({ 0.0f, 6.5f,-6.0f,0.0f }, { 0.0f,0.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f,0.0f });
		//DirectX::XMMatrixLookToLH({VEC3 pos},{VEC3 normalizedForward}, {VEC3 normalized updirection});

		DirectX::XMMATRIX projectionMatrix;
		projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(1.2217304764f, 16.0f / 9.0f, 1.0f, 50.0f);

		mViewProjectionMatrix = viewMatrix * projectionMatrix;
		
		mCBPassData.Initialize(mDevice.Get(), Utils::CalculateConstantbufferAlignment(sizeof(PassData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);

		//material allocations

		{
			mMaterialBuffers.reserve(4);

			mMaterialBuffers.emplace_back(D12Resource());
			mMaterialBuffers[0].Initialize(mDevice.Get(), Utils::CalculateConstantbufferAlignment(sizeof(MaterialCelShader)), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
			mMaterialBuffers[0]->SetName(L"Material CB 1");

			MaterialCelShader material;
			material.diffuseAlbedo = { .65f,0.0f,0.025f,1.0f };

			mBufferUploader.Upload((D12Resource*)mMaterialBuffers[0].GetAddressOf(), &material, sizeof(MaterialCelShader),
				(D12CommandList*)mCommandList.GetAddressOf(), (D12CommandQueue*)mCommandQueue.GetAddressOf(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

			mMaterialBuffers.emplace_back(D12Resource());
			mMaterialBuffers[1].Initialize(mDevice.Get(), Utils::CalculateConstantbufferAlignment(sizeof(MaterialCelShader)), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
			mMaterialBuffers[1]->SetName(L"Material CB 2");

			material;
			material.diffuseAlbedo = { .0f,0.0f,0.80f,1.0f };

			mBufferUploader.Upload((D12Resource*)mMaterialBuffers[1].GetAddressOf(), &material, sizeof(MaterialCelShader),
				(D12CommandList*)mCommandList.GetAddressOf(), (D12CommandQueue*)mCommandQueue.GetAddressOf(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);


			mMaterialBuffers.emplace_back(D12Resource());
			mMaterialBuffers[2].Initialize(mDevice.Get(), Utils::CalculateConstantbufferAlignment(sizeof(MaterialCelShader)), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
			mMaterialBuffers[2]->SetName(L"Material CB 3 (floor)");

			material;
			material.diffuseAlbedo = { .3f,0.3f,0.3f,1.0f };

			mBufferUploader.Upload((D12Resource*)mMaterialBuffers[2].GetAddressOf(), &material, sizeof(MaterialCelShader),
				(D12CommandList*)mCommandList.GetAddressOf(), (D12CommandQueue*)mCommandQueue.GetAddressOf(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);


			mMaterialBuffers.emplace_back(D12Resource());
			mMaterialBuffers[3].Initialize(mDevice.Get(), Utils::CalculateConstantbufferAlignment(sizeof(MaterialCelShader)), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
			mMaterialBuffers[3]->SetName(L"Material CB 4 (shadows)");

			material;
			material.diffuseAlbedo = { .0f,0.0f,0.0f,0.5f };

			mBufferUploader.Upload((D12Resource*)mMaterialBuffers[3].GetAddressOf(), &material, sizeof(MaterialCelShader),
				(D12CommandList*)mCommandList.GetAddressOf(), (D12CommandQueue*)mCommandQueue.GetAddressOf(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);



		}


		mLights[0].position = { 0.0f,0.0f,0.0f };
		mLights[0].strength = 1.0f;
		mLights[0].direction = { 0.0f,-1.0f,3.0f };
		
		//Transform allocations
		{

			mObjTransforms.reserve(3);
			mObjTransforms.emplace_back(D12Resource());
			mObjTransforms[0].Initialize(mDevice.Get(), Utils::CalculateConstantbufferAlignment(sizeof(ObjectData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
			mObjTransforms[0]->SetName(L"Transform 1 CB");

			ObjectData tempData;
			memcpy(mObjTransforms[0].GetCPUMemory(), &tempData, sizeof(ObjectData));

			mObjTransforms.emplace_back(D12Resource());
			mObjTransforms[1].Initialize(mDevice.Get(), Utils::CalculateConstantbufferAlignment(sizeof(ObjectData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
			mObjTransforms[1]->SetName(L"Transform 2 CB");
			tempData.transform.r[0] = { .3f,0.0f,1.0f,0.0f };

			tempData.transform.r[3] = { -3.0f,0.0f,-2.0f,0.0f };


			memcpy(mObjTransforms[1].GetCPUMemory(), &tempData, sizeof(ObjectData));

			mObjTransforms.emplace_back(D12Resource());
			mObjTransforms[2].Initialize(mDevice.Get(), Utils::CalculateConstantbufferAlignment(sizeof(ObjectData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
			mObjTransforms[2]->SetName(L"Transform 3 CB (floor)");

			tempData.transform = DirectX::XMMatrixIdentity();

			tempData.transform.r[0] = { 1000.0f,0.0,0.0,0.0f };
			tempData.transform.r[1] = { 0.0,0.3f,0.0,0.0f };
			tempData.transform.r[2] = { 0.0,0.0,1000.0f,0.0f };
			tempData.transform.r[3] = { 0.0f,-4.0f,0.0f,1.0f };

			memcpy(mObjTransforms[2].GetCPUMemory(), &tempData, sizeof(ObjectData));


		}

		//shadow transforms
		{


			mShadowTransform.resize(mObjTransforms.size());

			for (int i = 0; i < mObjTransforms.size(); i++) {

				mShadowTransform[i].Initialize(mDevice.Get(), Utils::CalculateConstantbufferAlignment(sizeof(ObjectData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);

				std::wstring name = L"Shadow transform CB ";

				name.append(std::to_wstring(i));

				mShadowTransform[i]->SetName(name.c_str());

				ObjectData tempData;

				memcpy(mShadowTransform[i].GetCPUMemory(), &tempData, sizeof(ObjectData));


			}



		}

		/*

		Projects:
		- Planar shadows
		- System: Timer / timestep
		- Custom move/copy operator on the D12Resource (bug fix)


		Ep. 26:
		- Implementing a timestep and perhaps some "animation" stuff
		- Do the bugfix here
		- Change the cpu read from the GPU memory into a different system
		
		*/




	}

	void RenderAPI::UpdateDraw()
	{
		memcpy(mCBPassData.GetCPUMemory(), &mViewProjectionMatrix, sizeof(PassData::viewproject));
		memcpy((BYTE*)mCBPassData.GetCPUMemory()+sizeof(PassData::viewproject), &mLights[0], sizeof(Light));


		DirectX::XMVECTOR planeToCastshadow = { 0.0f,1.0f,0.0f,0.0f };
		DirectX::XMVECTOR dirToLightSource = DirectX::XMVectorNegate(DirectX::XMLoadFloat3(&mLights[0].direction));
		DirectX::XMMATRIX shadowMatrix = DirectX::XMMatrixShadow(planeToCastshadow, dirToLightSource);
		DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, 0.001f, 0.0f);

		for (int i = 0; i < mShadowTransform.size(); i++) {

			DirectX::XMMATRIX temp = ((ObjectData*)mObjTransforms[i].GetCPUMemory())->transform * shadowMatrix * translation;

			ObjectData tempData;
			//bear in mind that it's kinda hacky to access the GPU memory and read from it using the CPU



			tempData.transform = temp;

			memcpy(mShadowTransform[i].GetCPUMemory(), &tempData, sizeof(ObjectData));

		}




		//Pre frame setup
		
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
		mCommandList.GFXCmd()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, 0);
		mCommandList.GFXCmd()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

		mCommandList.GFXCmd()->RSSetViewports(1, &mViewport);
		mCommandList.GFXCmd()->RSSetScissorRects(1, &mSRRect);

		//Actual draw passes:

		mCommandList.GFXCmd()->SetGraphicsRootSignature(mBasePipeline.GetRS());
		mCommandList.GFXCmd()->SetPipelineState(mBasePipeline.Get());
		mCommandList.GFXCmd()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		mCommandList.GFXCmd()->IASetVertexBuffers(0, 1, &mVBView);
		mCommandList.GFXCmd()->IASetIndexBuffer(&mIBView);
		mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(0, mCBPassData.Get()->GetGPUVirtualAddress());
		//Draw call
		{
			
			mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(1, mObjTransforms[0].Get()->GetGPUVirtualAddress());
			mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(2, mMaterialBuffers[0].Get()->GetGPUVirtualAddress());


			mCommandList.GFXCmd()->DrawIndexedInstanced(G_INDICES, 1, 0, 0, 0);
		}
	

		{
			
			mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(1, mObjTransforms[1].Get()->GetGPUVirtualAddress());
			mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(2, mMaterialBuffers[1].Get()->GetGPUVirtualAddress());


			mCommandList.GFXCmd()->DrawIndexedInstanced(G_INDICES, 1, 0, 0, 0);
		}

		//draw a floor
		{
			
			mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(1, mObjTransforms[2].Get()->GetGPUVirtualAddress());
			mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(2, mMaterialBuffers[2].Get()->GetGPUVirtualAddress());


			mCommandList.GFXCmd()->DrawIndexedInstanced(G_INDICES, 1, 0, 0, 0);
		}


		
		// Planar shadows pass

		mCommandList.GFXCmd()->SetGraphicsRootSignature(mPlanarShadowPipeline.GetRS());
		mCommandList.GFXCmd()->SetPipelineState(mPlanarShadowPipeline.Get());
		mCommandList.GFXCmd()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(0, mCBPassData.Get()->GetGPUVirtualAddress());

		//Draw call
		{
			
			mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(1, mShadowTransform[0].Get()->GetGPUVirtualAddress());
			mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(2, mMaterialBuffers[3].Get()->GetGPUVirtualAddress());


			mCommandList.GFXCmd()->DrawIndexedInstanced(G_INDICES, 1, 0, 0, 0);
		}


		{

			mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(1, mShadowTransform[1].Get()->GetGPUVirtualAddress());
			mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(2, mMaterialBuffers[3].Get()->GetGPUVirtualAddress());


			mCommandList.GFXCmd()->DrawIndexedInstanced(G_INDICES, 1, 0, 0, 0);
		}




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

		mCommandQueue.FlushQueue();

		for (int i = 0; i < mShadowTransform.size(); i++) {

			mShadowTransform[i].Release();
		}


		for (int i = 0; i < mMaterialBuffers.size(); i++) {
			mMaterialBuffers[i].Release();
		}



		for (int i = 0; i < mObjTransforms.size(); i++) {
			mObjTransforms[i].Release();
		}

		

		mVertexBuffer.Release();
		mIndexBuffer.Release();
		mCBPassData.Release();

		mBasePipeline.Release();
		mPlanarShadowPipeline.Release();
		mDepthDescHeap.Release();
		mDepthBuffer.Release();

		mBufferUploader.Release();

		

		
	
		mSwapChain.Release();
		
		mCommandList.Release();

		mCommandQueue.Release();


		if (mDevice.Get()) {

			mDevice.Reset();

		}
	}

}




/*

verticesbox[1].position = { -1.000000, - 1.000000, 1.000000 };
verticesbox[2].position = { -1.000000, 1.000000, - 1.00000 };
verticesbox[3].position = { -1.000000, - 1.000000, - 1.000000 };

verticesbox[4].position = { 1.000000, 1.000000, 1.000000 };
//verticesbox[4].color = { 1.0f,0.0f,0.0f,1.0f };
verticesbox[5].position = { 1.000000, - 1.000000, 1.000000 };
//verticesbox[5].color = { 1.0f,0.0f,0.0f,1.0f };
verticesbox[6].position = { 1.000000, 1.000000, - 1.00000 };
//verticesbox[6].color = { 1.0f,0.0f,0.0f,1.0f };
verticesbox[7].position = { 1.000000, - 1.000000, - 1.000000 };
//verticesbox[7].color = { 1.0f,0.0f,0.0f,1.0f };
*/


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