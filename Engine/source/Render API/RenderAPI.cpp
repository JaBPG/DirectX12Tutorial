#include "pch.h"
#include "RenderAPI.h"



#include "DirectX12/DXGI/DXGIFactory.h"
#include "DirectX12/DXGI/DXGIAdapter.h"


#include "DirectX12/Debug/D12Debug.h"


#include "../Utilities/Utilities.h"





namespace Engine {
	using namespace Render;


	RenderAPI::~RenderAPI()
	{

		/* TODO IMPROVE RELEASE FUNCTION TO CLEAN UP OBJECTS */
		Release();
	}

	void RenderAPI::Initialize(HWND hwnd, const UINT width, const UINT height)
	{
		mWidth = width;
		mHeight = height;

		constexpr unsigned int shadowmapresolution = 2048;

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

		/* ESSENTIALS */
		{
			mDevice.Init(adapter.Get());
			mDevice->SetName(L"Main virtual device");

			mCommandQueue.Initialize(mDevice.Get());
			mCommandList.Initialize(mDevice.Get());

			mSwapChain.Initialize(mDevice.Get(), factory.Get(), mCommandQueue.Get(), hwnd, mWidth, mHeight);
			mBufferUploader.Initialize(mDevice.Get(), MBs(32));

		}

		/* LOAD OUR FBX MODELS */
		{
			std::vector<Vertex> vertices;
			std::vector<UINT32> indices;

			mModelLoader.LoadFBXModel("models/test.fbx", vertices, indices, mMeshes);


			mVertexBuffer.Initialize(mDevice.Get(), MBs(64), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
			mVertexBuffer.Get()->SetName(L"Vertex buffer");

			mBufferUploader.Upload((D12Resource*)mVertexBuffer.GetAddressOf(), vertices.data(), sizeof(Vertex) * vertices.size(),
				(D12CommandList*)mCommandList.GetAddressOf(), (D12CommandQueue*)mCommandQueue.GetAddressOf(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

			mVBView.BufferLocation = mVertexBuffer.Get()->GetGPUVirtualAddress();
			mVBView.StrideInBytes = sizeof(Vertex);
			mVBView.SizeInBytes = MBs(64);

			mIndexBuffer.Initialize(mDevice.Get(), MBs(64), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
			mIndexBuffer->SetName(L"Index buffer");

			mBufferUploader.Upload((D12Resource*)mIndexBuffer.GetAddressOf(), indices.data(), sizeof(UINT32) * indices.size(),
				(D12CommandList*)mCommandList.GetAddressOf(), (D12CommandQueue*)mCommandQueue.GetAddressOf());
			mIBView.BufferLocation = mIndexBuffer.Get()->GetGPUVirtualAddress();
			mIBView.Format = DXGI_FORMAT_R32_UINT;
			mIBView.SizeInBytes = MBs(64);
		}
		

		/* DESCRIPTOR HEAPS */
		{
			mDeferredRenderTargetsDescHeap.InitializeRTV(mDevice.Get(), 3); //The deferred pipeline uses 3 rendertagets currently
			mDeferredRenderTargetsDescHeap->SetName(L"Deferred Descriptor heap");
			mCBSRVUAVDescHeap.InitializeCBSRVUAV(mDevice.Get(), 5); //should hold our shadowmap stuff in the future, so should have another name eventually and bigger size
			mCBSRVUAVDescHeap->SetName(L"Main CB, SRV, UAV Descriptor heap");

			mDepthDescHeap.InitializeDepthHeap(mDevice.Get(), 2);
			mDepthDescHeap->SetName(L"Depth descriptor heap");

		}

		/* deferred pipeline stuff */
		{
			mDeferredPipeline.InitializeDeferred(mDevice.Get());
			mDeferredPipeline->SetName(L"Deferred pipeline PSO");

			mDeferredPixelPipeline.InitializePixelProces(mDevice.Get(), L"shaders/postfx/deferredlights/PS.hlsl");
			mDeferredPixelPipeline->SetName(L"Deferred pixel pipeline PSO");

			D3D12_RESOURCE_BARRIER barrier = {};
			{
				barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				barrier.Transition.Subresource = 0;
				barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
				barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			}
			const unsigned int rendertargets = 3;

			/* create our deferred buffers (G-BUFFER COMPONENTS) */
			for (int i = 0; i < rendertargets; i++) {

				mDeferredOutputTextures[i].InitializeAsTexture(mDevice.Get(), width, height, DXGI_FORMAT_R32G32B32A32_FLOAT);

				barrier.Transition.pResource = mDeferredOutputTextures[i].Get();
				mCommandList.GFXCmd()->ResourceBarrier(1, &barrier);

				mDevice->CreateRenderTargetView(mDeferredOutputTextures[i].Get(), nullptr, mDeferredRenderTargetsDescHeap.GetCPUHandle(i));
				mDevice->CreateShaderResourceView(mDeferredOutputTextures[i].Get(), &mDeferredOutputTextures[i].GetSRV(), mCBSRVUAVDescHeap.GetCPUHandle(i));

				std::wstring name = L"G-BUFFER rendertarget #";
				name.append(std::to_wstring(i + 1));
				mDeferredOutputTextures[i]->SetName(name.c_str());
			}
		}


		/* shadow map pipeline */

		{
			mShadowMapPipeline.InitializeShadowMapping(mDevice.Get());
			mShadowMapPipeline->SetName(L"Shadow map pipeline");

		}



		/* old stuff, will be removed */
		//mBasePipeline.Initialize(mDevice.Get());
		//mPlanarShadowPipeline.InitializeAsTransparent(mDevice.Get());

		/* DEPTH BUFFERS */
		{
			/* regular */
			D3D12_SHADER_RESOURCE_VIEW_DESC texturedesc = {};
			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};

			{
				mDepthBuffers[0].InitializeAsDepthBuffer(mDevice.Get(), mWidth, mHeight);
				mDepthBuffers[0]->SetName(L"G-BUFFER depth buffer");

				dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				dsvDesc.Texture2D.MipSlice = 0;
				dsvDesc.Flags = D3D12_DSV_FLAG_NONE;


				texturedesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				texturedesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				texturedesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

				texturedesc.Texture2D.MipLevels = 1;
				texturedesc.Texture2D.MostDetailedMip = 0;
				texturedesc.Texture2D.PlaneSlice = 0;
				texturedesc.Texture2D.ResourceMinLODClamp = 0.0f;


				mDevice->CreateDepthStencilView(mDepthBuffers[0].Get(), &dsvDesc, mDepthDescHeap.GetCPUHandle(0));
				mDevice->CreateShaderResourceView(mDepthBuffers[0].Get(), &texturedesc, mCBSRVUAVDescHeap.GetCPUHandle(3));
			}
			


			/* shadow map depth buffer */

			{
				mDepthBuffers[1].InitializeAsDepthBuffer(mDevice.Get(), shadowmapresolution, shadowmapresolution, DXGI_FORMAT_D32_FLOAT);
				mDepthBuffers[1]->SetName(L"ShadowMap Depthbuffer");

				dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
				dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				dsvDesc.Texture2D.MipSlice = 0;
				dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

				texturedesc.Format = DXGI_FORMAT_R32_FLOAT;
				texturedesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				texturedesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

				texturedesc.Texture2D.MipLevels = 1;
				texturedesc.Texture2D.MostDetailedMip = 0;
				texturedesc.Texture2D.PlaneSlice = 0;
				texturedesc.Texture2D.ResourceMinLODClamp = 0.0f;

				mDevice->CreateDepthStencilView(mDepthBuffers[1].Get(), &dsvDesc, mDepthDescHeap.GetCPUHandle(1));
				mDevice->CreateShaderResourceView(mDepthBuffers[1].Get(), &texturedesc, mCBSRVUAVDescHeap.GetCPUHandle(4));
			}
			

		}
		
		/* viewport and scissor rect */
		{

			/* regular viewport */
			mViewport[0].TopLeftX = 0;
			mViewport[0].TopLeftY = 0;
			mViewport[0].Width = mWidth;
			mViewport[0].Height = mHeight;
			mViewport[0].MinDepth = 0.0f;
			mViewport[0].MaxDepth = 1.0f;

			mSRRect[0].left = 0;
			mSRRect[0].right = mViewport[0].Width;
			mSRRect[0].top = 0;
			mSRRect[0].bottom = mViewport[0].Height;


			/* shadowmap viewport */
			mViewport[1].TopLeftX = 0;
			mViewport[1].TopLeftY = 0;
			mViewport[1].Width = shadowmapresolution;
			mViewport[1].Height = shadowmapresolution;
			mViewport[1].MinDepth = 0.0f;
			mViewport[1].MaxDepth = 1.0f;

			mSRRect[1].left = 0;
			mSRRect[1].right = mViewport[1].Width;
			mSRRect[1].top = 0;
			mSRRect[1].bottom = mViewport[1].Height;

		}


		/* camera matrix stuff */
		{
			DirectX::XMMATRIX viewMatrix;
			viewMatrix = DirectX::XMMatrixLookAtLH({ 0.0f, 5.5f,-19.0f,0.0f }, { 0.0f,0.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f,0.0f });
			//DirectX::XMMatrixLookToLH({VEC3 pos},{VEC3 normalizedForward}, {VEC3 normalized updirection});

			DirectX::XMMATRIX projectionMatrix;
			projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(1.2217304764f, 16.0f / 9.0f, 1.0f, 50.0f);

			mViewProjectionMatrix = viewMatrix * projectionMatrix;

			mCBPassData.Initialize(mDevice.Get(), Utils::CalculateConstantbufferAlignment(sizeof(PassData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
			mCBPassData->SetName(L"PassData CB");
		}


		//material allocations

		{
			

			mMaterialBuffers.emplace_back(D12Resource());
			mMaterialBuffers[0].Initialize(mDevice.Get(), Utils::CalculateConstantbufferAlignment(sizeof(MaterialCelShader)), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
			mMaterialBuffers[0]->SetName(L"Material CB 1 skeleton");

			MaterialCelShader material;
			material.diffuseAlbedo = { .45f,0.45f,0.45f,1.0f };

			mBufferUploader.Upload((D12Resource*)mMaterialBuffers[0].GetAddressOf(), &material, sizeof(MaterialCelShader),
				(D12CommandList*)mCommandList.GetAddressOf(), (D12CommandQueue*)mCommandQueue.GetAddressOf(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

			mMaterialBuffers.emplace_back(D12Resource());
			mMaterialBuffers[1].Initialize(mDevice.Get(), Utils::CalculateConstantbufferAlignment(sizeof(MaterialCelShader)), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
			mMaterialBuffers[1]->SetName(L"Material CB text");

			//material.diffuseAlbedo = { .55f,0.0f,0.015f,1.0f };
			
			//material.diffuseAlbedo = { .8,0.8f,0.8f,1.0f };
			material.diffuseAlbedo = { .10,0.17f,0.0f,1.0f };
			

			mBufferUploader.Upload((D12Resource*)mMaterialBuffers[1].GetAddressOf(), &material, sizeof(MaterialCelShader),
				(D12CommandList*)mCommandList.GetAddressOf(), (D12CommandQueue*)mCommandQueue.GetAddressOf(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);


			mMaterialBuffers.emplace_back(D12Resource());
			mMaterialBuffers[2].Initialize(mDevice.Get(), Utils::CalculateConstantbufferAlignment(sizeof(MaterialCelShader)), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
			mMaterialBuffers[2]->SetName(L"Material CB 3 (floor)");

			//material.diffuseAlbedo = { .6f,0.3f,0.0f,1.0f };
			material.diffuseAlbedo = { .05f,0.05f,0.05f,1.0f };

			mBufferUploader.Upload((D12Resource*)mMaterialBuffers[2].GetAddressOf(), &material, sizeof(MaterialCelShader),
				(D12CommandList*)mCommandList.GetAddressOf(), (D12CommandQueue*)mCommandQueue.GetAddressOf(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

			mMaterialBuffers.emplace_back(D12Resource());
			mMaterialBuffers[3].Initialize(mDevice.Get(), Utils::CalculateConstantbufferAlignment(sizeof(MaterialCelShader)), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON);
			mMaterialBuffers[3]->SetName(L"Material CB 4 (other object)");
			
			
			material.diffuseAlbedo = { 0.33f,0.0f,0.19f,1.0f };
			material.diffuseAlbedo = { 0.24f,0.0f,0.10f,1.0f };
			
			//material.diffuseAlbedo = { .0f,0.0f,0.80f,1.0f };
			//material.diffuseAlbedo = { 0.7f,0.3f,0.1f,1.0f };

			mBufferUploader.Upload((D12Resource*)mMaterialBuffers[3].GetAddressOf(), &material, sizeof(MaterialCelShader),
				(D12CommandList*)mCommandList.GetAddressOf(), (D12CommandQueue*)mCommandQueue.GetAddressOf(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);



		}

		mLights[0].position = { 0.0f,0.0f,0.0f };
		mLights[0].strength = 1.0f;
		mLights[0].direction = { 0.3f,-0.4f,0.5f };
		
		//Transform allocations
		{
			mObjTransforms.emplace_back(D12Resource());
			mObjTransforms[0].Initialize(mDevice.Get(), Utils::CalculateConstantbufferAlignment(sizeof(ObjectData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
			mObjTransforms[0]->SetName(L"Transform 1 CB");

			ObjectData tempData;
			
			DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(4.0, 4.0f, 4.0f);
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationAxis({0.0f,1.0f,0.0f}, 3.14f);
			DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, 0.0f, -8.0f);
		

			tempData.transform = scale * rotation * translation;
			mObjTransformsCPU.push_back(tempData);

			memcpy(mObjTransforms[0].GetCPUMemory(), &tempData, sizeof(ObjectData));

			mObjTransforms.emplace_back(D12Resource());
			mObjTransforms[1].Initialize(mDevice.Get(), Utils::CalculateConstantbufferAlignment(sizeof(ObjectData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
			mObjTransforms[1]->SetName(L"Transform 2 text");

			scale = DirectX::XMMatrixScaling(3.0, 3.0f, 1.5f);
			//rotation = DirectX::XMMatrixRotationAxis({ 0.0f,1.0f,0.0f }, 3.14f);
			translation = DirectX::XMMatrixTranslation(0.0f, 0.0f, -12.0f);


			tempData.transform = scale * translation;

			mObjTransformsCPU.push_back(tempData);

			memcpy(mObjTransforms[1].GetCPUMemory(), &tempData, sizeof(ObjectData));

			mObjTransforms.emplace_back(D12Resource());
			mObjTransforms[2].Initialize(mDevice.Get(), Utils::CalculateConstantbufferAlignment(sizeof(ObjectData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
			mObjTransforms[2]->SetName(L"Transform 3 CB (floor)");

			tempData.transform = DirectX::XMMatrixIdentity();

			tempData.transform.r[0] = { 1000.0f,0.0,0.0,0.0f };
			tempData.transform.r[1] = { 0.0,1.0f,0.0,0.0f };
			tempData.transform.r[2] = { 0.0,0.0,1000.0f,0.0f };
			tempData.transform.r[3] = { 0.0f,-1.0f,0.0f,1.0f };

			mObjTransformsCPU.push_back(tempData);

			memcpy(mObjTransforms[2].GetCPUMemory(), &tempData, sizeof(ObjectData));



			mObjTransforms.emplace_back(D12Resource());
			mObjTransforms[3].Initialize(mDevice.Get(), Utils::CalculateConstantbufferAlignment(sizeof(ObjectData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
			mObjTransforms[3]->SetName(L"Transform 3 CB (desk)");

			scale = DirectX::XMMatrixScaling(.03, 0.03f, 0.03f);
			rotation = DirectX::XMMatrixRotationAxis({ 1.0f,0.0f,0.0f }, 4.71f);
			DirectX::XMMATRIX rotation2 = DirectX::XMMatrixRotationAxis({ 0.0f,1.0f,0.0f }, 2.34f);

			rotation = rotation * rotation2;
			translation = DirectX::XMMatrixTranslation(-9.0f, 0.35f, -5.0f);
			tempData.transform = scale * rotation * translation;

			mObjTransformsCPU.push_back(tempData);

			memcpy(mObjTransforms[3].GetCPUMemory(), &tempData, sizeof(ObjectData));


		}

		//Shadow transforms
		{

			/*
			mShadowTransforms.resize(mObjTransforms.size());


			for (int i = 0; i < mObjTransforms.size(); i++) {
				mShadowTransforms[i].Initialize(mDevice.Get(), Utils::CalculateConstantbufferAlignment(sizeof(ObjectData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
				std::wstring name = L"Shadow transform buffer ";
				name.append(std::to_wstring(i));

				mShadowTransforms[i]->SetName(name.c_str());

				ObjectData tempData;

				memcpy(mShadowTransforms[i].GetCPUMemory(), &tempData, sizeof(ObjectData));


			}
			*/


		}

	}

	void RenderAPI::UpdateDraw(const float ts)
	{

		Light currentFrameLights = mLights[0];
		PassData passData;



		//Simulation
		{
		
			float increment = 0.05f * ts;

			if (mLightCycle >= 1.0f) {
				mLightCycle = 0.0f;
			}
		
			mLightCycle += increment;


			float clampedCycle = (mLightCycle * 2.0f) - 1.0f; // this will set it between -1 and 1 (with 0.0 = midday)
			float maxrotation = 3.14;// / 2.0f;
			float actualrotation = maxrotation * clampedCycle;

			DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationNormal({ 0.0f,0.0f,1.0f,0.0f }, actualrotation);

			DirectX::XMVECTOR frameLightDirection = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&mLights[0].direction), rotationMatrix);

			//DirectX::XMStoreFloat3(&currentFrameLights.direction, frameLightDirection);

			/* Shadowmap computations */
			{
				float shadowmapradius = 50.0f;

				DirectX::XMVECTOR scenecenter = { 0.0f,0.0f,0.0f };
				DirectX::XMVECTOR lightposition = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&currentFrameLights.direction),-shadowmapradius);

				DirectX::XMMATRIX viewlights = DirectX::XMMatrixLookAtLH(lightposition, scenecenter, { 0.0f,1.0f,0.0f,0.0f });

				DirectX::XMMATRIX projectionlight = DirectX::XMMatrixOrthographicLH(20, 20, 0.5f, 200.0f);

				passData.lightviewproject = viewlights * projectionlight;

			}

		}

		//Update buffers
		{
			passData.viewproject = mViewProjectionMatrix;
			passData.scenelight = currentFrameLights;

			memcpy(mCBPassData.GetCPUMemory(), &passData, sizeof(PassData));
		}

		ID3D12DescriptorHeap* descriptorheaps[1] = { mCBSRVUAVDescHeap.Get() };
		mCommandList.GFXCmd()->SetDescriptorHeaps(1, descriptorheaps);

		//Draw to shadowmap
		{

			mCommandList.GFXCmd()->RSSetViewports(1, &mViewport[1]);
			mCommandList.GFXCmd()->RSSetScissorRects(1, &mSRRect[1]);


			mCommandList.GFXCmd()->SetGraphicsRootSignature(mShadowMapPipeline.GetRS());
			mCommandList.GFXCmd()->SetPipelineState(mShadowMapPipeline.Get());
			mCommandList.GFXCmd()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			mCommandList.GFXCmd()->IASetVertexBuffers(0, 1, &mVBView);
			mCommandList.GFXCmd()->IASetIndexBuffer(&mIBView);

			/* clear depth buffer for the shadowmap */
			{

				D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = mDepthDescHeap.GetCPUHandle(1); //shadow map depth buffer here
			
				mCommandList.GFXCmd()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, 0);
				mCommandList.GFXCmd()->OMSetRenderTargets(0, nullptr, false, &dsvHandle);				
			}

			mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(0, mCBPassData.Get()->GetGPUVirtualAddress());
			//skeleton
			{
				mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(1, mObjTransforms[0].Get()->GetGPUVirtualAddress());
				mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(2, mMaterialBuffers[0].Get()->GetGPUVirtualAddress());

				Render::MeshDrawData* drawdata = &mMeshes[1];

				mCommandList.GFXCmd()->DrawIndexedInstanced(drawdata->indexcount, 1, drawdata->indexoffset, drawdata->vertexoffset, 0);
			}
			//text
			{
				mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(1, mObjTransforms[1].Get()->GetGPUVirtualAddress());
				mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(2, mMaterialBuffers[1].Get()->GetGPUVirtualAddress());

				Render::MeshDrawData* drawdata = &mMeshes[3];

				mCommandList.GFXCmd()->DrawIndexedInstanced(drawdata->indexcount, 1, drawdata->indexoffset, drawdata->vertexoffset, 0);
			}


			//cupboard
			{
				mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(1, mObjTransforms[3].Get()->GetGPUVirtualAddress());
				mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(2, mMaterialBuffers[3].Get()->GetGPUVirtualAddress());

				Render::MeshDrawData* drawdata = &mMeshes[2];

				mCommandList.GFXCmd()->DrawIndexedInstanced(drawdata->indexcount, 1, drawdata->indexoffset, drawdata->vertexoffset, 0);
			}



		}

		//Draw frame
		{
			mCommandList.GFXCmd()->RSSetViewports(1, &mViewport[0]);
			mCommandList.GFXCmd()->RSSetScissorRects(1, &mSRRect[0]);

	
			mCommandList.GFXCmd()->SetGraphicsRootSignature(mDeferredPipeline.GetRS());
			mCommandList.GFXCmd()->SetPipelineState(mDeferredPipeline.Get());
			mCommandList.GFXCmd()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			mCommandList.GFXCmd()->IASetVertexBuffers(0, 1, &mVBView);
			mCommandList.GFXCmd()->IASetIndexBuffer(&mIBView);


			const float clearColor[] = { 0.0f,0.0f,0.0f,0.0f };

			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle[3] = {	mDeferredRenderTargetsDescHeap.GetCPUHandle(0),
															mDeferredRenderTargetsDescHeap.GetCPUHandle(1),
															mDeferredRenderTargetsDescHeap.GetCPUHandle(2) };

			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = mDepthDescHeap.GetCPUHandle(0);

			mCommandList.GFXCmd()->ClearRenderTargetView(rtvHandle[0], clearColor, 0, 0);
			mCommandList.GFXCmd()->ClearRenderTargetView(rtvHandle[1], clearColor, 0, 0);
			mCommandList.GFXCmd()->ClearRenderTargetView(rtvHandle[2], clearColor, 0, 0);

			mCommandList.GFXCmd()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, 0);
			mCommandList.GFXCmd()->OMSetRenderTargets(3, rtvHandle, false, &dsvHandle);

			mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(0, mCBPassData.Get()->GetGPUVirtualAddress());




			//floor 
			{

				mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(1, mObjTransforms[2].Get()->GetGPUVirtualAddress());
				mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(2, mMaterialBuffers[2].Get()->GetGPUVirtualAddress());

				Render::MeshDrawData* drawdata = &mMeshes[0];

				mCommandList.GFXCmd()->DrawIndexedInstanced(drawdata->indexcount, 1, drawdata->indexoffset, drawdata->vertexoffset, 0);

			}

			//skeleton
			{
				mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(1, mObjTransforms[0].Get()->GetGPUVirtualAddress());
				mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(2, mMaterialBuffers[0].Get()->GetGPUVirtualAddress());

				Render::MeshDrawData* drawdata = &mMeshes[1];

				mCommandList.GFXCmd()->DrawIndexedInstanced(drawdata->indexcount, 1, drawdata->indexoffset, drawdata->vertexoffset, 0);
			}
			//text
			{
				mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(1, mObjTransforms[1].Get()->GetGPUVirtualAddress());
				mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(2, mMaterialBuffers[1].Get()->GetGPUVirtualAddress());

				Render::MeshDrawData* drawdata = &mMeshes[3];

				mCommandList.GFXCmd()->DrawIndexedInstanced(drawdata->indexcount, 1, drawdata->indexoffset, drawdata->vertexoffset, 0);
			}


			//cupboard
			{
				mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(1, mObjTransforms[3].Get()->GetGPUVirtualAddress());
				mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(2, mMaterialBuffers[3].Get()->GetGPUVirtualAddress());

				Render::MeshDrawData* drawdata = &mMeshes[2];

				mCommandList.GFXCmd()->DrawIndexedInstanced(drawdata->indexcount, 1, drawdata->indexoffset, drawdata->vertexoffset, 0);
			}

		
			D3D12_RESOURCE_BARRIER barrier = {};

		

			/* Set our swapchain backbuffer in an rendertarget state*/
			{
				barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				barrier.Transition.pResource = mSwapChain.GetCurrentRenderTarget();
				barrier.Transition.Subresource = 0;
				barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
				barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

				mCommandList.GFXCmd()->ResourceBarrier(1, &barrier);
			}

			mCommandList.GFXCmd()->SetGraphicsRootSignature(mDeferredPixelPipeline.GetRS());
			mCommandList.GFXCmd()->SetPipelineState(mDeferredPixelPipeline.Get());
			mCommandList.GFXCmd()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			mCommandList.GFXCmd()->IASetVertexBuffers(0, 1, &mVBView);
			mCommandList.GFXCmd()->IASetIndexBuffer(&mIBView);


			/* clear the swapchain and depth buffer and bind it as output */
			{
				const float clearColor[] = { 0.0f,0.0f,0.0f,1.0f };
				D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = mSwapChain.GetCurrentRTVHandle();
			
				mCommandList.GFXCmd()->ClearRenderTargetView(rtvHandle, clearColor, 0, 0);
				mCommandList.GFXCmd()->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
			}

			mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(0, mCBPassData.Get()->GetGPUVirtualAddress());
			mCommandList.GFXCmd()->SetGraphicsRootDescriptorTable(3, mCBSRVUAVDescHeap.GetGPUHandle(0));

			unsigned int size[2] = { 1280,720 };

			//mCommandList.GFXCmd()->SetComputeRoot32BitConstants(4, 2, &size, 0);


			mCommandList.GFXCmd()->DrawIndexedInstanced(3, 1, 0, 0, 0);


			/* Transition the swapchain backbuffer into a present state */
			{
				barrier = {};
				barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				barrier.Transition.pResource = mSwapChain.GetCurrentRenderTarget();
				barrier.Transition.Subresource = 0;
				barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
				barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

				mCommandList.GFXCmd()->ResourceBarrier(1, &barrier);
			}

			
			/* Close command list and execute + present */

			{
				mCommandList.GFXCmd()->Close();
				mCommandQueue.M_ExecuteCommandList(mCommandList.Get());

				mSwapChain.Present();

				while (mCommandQueue.GetFence()->GetCompletedValue() < mCommandQueue.M_GetCurrentFenceValue()) {

					_mm_pause();
				}

				mCommandList.ResetCommandList();
			}

		}
		
	}

	void RenderAPI::Release()
	{

		/* Fill out this stuff */

		mCommandQueue.FlushQueue();

		for (int i = 0; i < mMaterialBuffers.size(); i++) {
			mMaterialBuffers[i].Release();
		}



		for (int i = 0; i < mObjTransforms.size(); i++) {
			mObjTransforms[i].Release();
		}

		

		mVertexBuffer.Release();
		mIndexBuffer.Release();
		mCBPassData.Release();

		//mBasePipeline.Release();
		//mPlanarShadowPipeline.Release();
		mDepthDescHeap.Release();
		mDepthBuffers[0].Release();
		mDepthBuffers[1].Release();

		mBufferUploader.Release();
	
		mSwapChain.Release();
		
		mCommandList.Release();

		mCommandQueue.Release();


		if (mDevice.Get()) {

			mDevice.Reset();

		}
	}

}




//memcpy((BYTE*)mCBPassData.GetCPUMemory() + sizeof(PassData::viewproject)*2, &currentFrameLights, sizeof(Light));

/*
DirectX::XMVECTOR planeToCastShadow = { 0.0f,1.0f,0.0f,0.0f };
DirectX::XMVECTOR dirToLightSource = DirectX::XMVectorNegate(DirectX::XMLoadFloat3(&currentFrameLights.direction));

DirectX::XMMATRIX shadowMatrix = DirectX::XMMatrixShadow(planeToCastShadow, dirToLightSource);
DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, 0.001f, 0.0f);


for (int i = 0; i < mShadowTransforms.size(); i++) {
	ObjectData tempData;
	tempData.transform = mObjTransformsCPU[i].transform * shadowMatrix * translation;
	memcpy(mShadowTransforms[i].GetCPUMemory(), &tempData, sizeof(ObjectData));

}
*/


/*

mCommandList.GFXCmd()->SetGraphicsRootSignature(mBasePipeline.GetRS());
mCommandList.GFXCmd()->SetPipelineState(mBasePipeline.Get());
mCommandList.GFXCmd()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

mCommandList.GFXCmd()->IASetVertexBuffers(0, 1, &mVBView);
mCommandList.GFXCmd()->IASetIndexBuffer(&mIBView);
mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(0, mCBPassData.Get()->GetGPUVirtualAddress());

*/
//Draw call

/*
{

	mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(1, mObjTransforms[0].Get()->GetGPUVirtualAddress());
	mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(2, mMaterialBuffers[0].Get()->GetGPUVirtualAddress());

	Render::MeshDrawData* drawdata = &mMeshes[0];

	mCommandList.GFXCmd()->DrawIndexedInstanced(drawdata->indexcount, 1, drawdata->indexoffset, drawdata->vertexoffset, 0);

}


{

	mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(1, mObjTransforms[1].Get()->GetGPUVirtualAddress());
	mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(2, mMaterialBuffers[1].Get()->GetGPUVirtualAddress());

	Render::MeshDrawData* drawdata = &mMeshes[1];

	mCommandList.GFXCmd()->DrawIndexedInstanced(drawdata->indexcount, 1, drawdata->indexoffset, drawdata->vertexoffset, 0);

}

//draw a floor
{

	mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(1, mObjTransforms[2].Get()->GetGPUVirtualAddress());
	mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(2, mMaterialBuffers[2].Get()->GetGPUVirtualAddress());

	Render::MeshDrawData* drawdata = &mMeshes[2];

	mCommandList.GFXCmd()->DrawIndexedInstanced(drawdata->indexcount, 1, drawdata->indexoffset, drawdata->vertexoffset, 0);

}

mCommandList.GFXCmd()->SetPipelineState(mPlanarShadowPipeline.Get());
mCommandList.GFXCmd()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


//Draw call
{

	mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(1, mShadowTransforms[0].Get()->GetGPUVirtualAddress());
	mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(2, mMaterialBuffers[3].Get()->GetGPUVirtualAddress());

	Render::MeshDrawData* drawdata = &mMeshes[0];

	//mCommandList.GFXCmd()->DrawIndexedInstanced(drawdata->indexcount, 1, drawdata->indexoffset, drawdata->vertexoffset, 0);

}


{

	mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(1, mShadowTransforms[1].Get()->GetGPUVirtualAddress());
	mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(2, mMaterialBuffers[3].Get()->GetGPUVirtualAddress());

	Render::MeshDrawData* drawdata = &mMeshes[0];

	//mCommandList.GFXCmd()->DrawIndexedInstanced(drawdata->indexcount, 1, drawdata->indexoffset, drawdata->vertexoffset, 0);

}

*/

/*
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

			*/
/*
		*
		#define G_BOX_VERTICES 24#define G_BOX_VERTICES 24
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
		*/


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