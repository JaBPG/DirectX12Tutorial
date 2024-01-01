#include "pch.h"
#include "D12PipelineState.h"




namespace Engine {



	D12PipelineState::~D12PipelineState()
	{
		
		Release();
	}

	void D12PipelineState::Initialize(ID3D12Device* pDevice)
	{

		mRootSignature.Initialize(pDevice);
		mShaders[0].Initialize(L"shaders/VS.hlsl", HLSLShader::ShaderType::VERTEX);
		mShaders[1].Initialize(L"shaders/PS.hlsl", HLSLShader::ShaderType::PIXEL);

		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature = mRootSignature.Get();
		desc.VS.pShaderBytecode = mShaders[0].GetByteBlob()->GetBufferPointer();
		desc.VS.BytecodeLength = mShaders[0].GetByteBlob()->GetBufferSize();
		desc.PS.pShaderBytecode = mShaders[1].GetByteBlob()->GetBufferPointer();
		desc.PS.BytecodeLength = mShaders[1].GetByteBlob()->GetBufferSize();

		desc.BlendState.AlphaToCoverageEnable = false;
		desc.BlendState.IndependentBlendEnable = false;
		desc.BlendState.RenderTarget[0].BlendEnable = false;
		desc.BlendState.RenderTarget[0].LogicOpEnable = false;
		desc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		desc.SampleMask = 0xFFFFFFFF;
		desc.SampleDesc = { 1,0 };

		//desc.RasterizerState = {};

		desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		desc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; //might do none
		desc.RasterizerState.FrontCounterClockwise = true;
		desc.RasterizerState.DepthClipEnable = true;
		desc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.DepthStencilState.DepthEnable = true;
		desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;


		D3D12_INPUT_ELEMENT_DESC elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 0 },
			 { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		/*
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		*/

		D3D12_INPUT_LAYOUT_DESC inputLayout = {};

		inputLayout.NumElements = 2;
		inputLayout.pInputElementDescs = elements;


		desc.InputLayout = inputLayout;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;


		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.NodeMask = 0;
		desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
				
		YT_EVAL_HR(pDevice->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(GetAddressOf())), "Error creating the pipeline state object");

	}

	void D12PipelineState::InitializeAsTransparent(ID3D12Device* pDevice)
	{



		mRootSignature.Initialize(pDevice);
		mShaders[0].Initialize(L"shaders/pshadow/VS.hlsl", HLSLShader::ShaderType::VERTEX);
		mShaders[1].Initialize(L"shaders/pshadow/PS.hlsl", HLSLShader::ShaderType::PIXEL);

		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature = mRootSignature.Get();
		desc.VS.pShaderBytecode = mShaders[0].GetByteBlob()->GetBufferPointer();
		desc.VS.BytecodeLength = mShaders[0].GetByteBlob()->GetBufferSize();
		desc.PS.pShaderBytecode = mShaders[1].GetByteBlob()->GetBufferPointer();
		desc.PS.BytecodeLength = mShaders[1].GetByteBlob()->GetBufferSize();

		desc.BlendState.AlphaToCoverageEnable = false;
		desc.BlendState.IndependentBlendEnable = false;
		desc.BlendState.RenderTarget[0].BlendEnable = true;
		desc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;


		desc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		desc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		desc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

		//source color: gets multiplied by source alpha 
		//destination color: gets multipied by the inverse source alpha (1-source alpha)
		//add the colors together


		//maybe we have to set some alpha blending, but probably not

		desc.BlendState.RenderTarget[0].LogicOpEnable = false;
		desc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		desc.SampleMask = 0xFFFFFFFF;
		desc.SampleDesc = { 1,0 };


		desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		desc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; 
		desc.RasterizerState.FrontCounterClockwise = true;
		desc.RasterizerState.DepthClipEnable = true;
		desc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.DepthStencilState.DepthEnable = true;
		desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

		desc.DepthStencilState.StencilEnable = true;
		desc.DepthStencilState.StencilReadMask = 0xFF;
		desc.DepthStencilState.StencilWriteMask = 0xFF;
		desc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
		desc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		desc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
		desc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		

		//this depends on our culling method
		desc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
		desc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		desc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
		desc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;


		D3D12_INPUT_ELEMENT_DESC elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 0 },
			 { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};


		D3D12_INPUT_LAYOUT_DESC inputLayout = {};

		inputLayout.NumElements = 2;
		inputLayout.pInputElementDescs = elements;


		desc.InputLayout = inputLayout;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;


		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.NodeMask = 0;
		desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		YT_EVAL_HR(pDevice->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(GetAddressOf())), "Error creating the pipeline state object");
		
	}


	void D12PipelineState::Release()
	{
		mRootSignature.Release();
		mShaders[0].Release();
		mShaders[1].Release();

		if (Get()) {
			Reset();
		}
	}

}