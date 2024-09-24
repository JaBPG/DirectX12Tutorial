#include "pch.h"
#include "D12RootSignature.h"



namespace Engine {

	D12RootSignature::~D12RootSignature()
	{
		Release();
	}
	void D12RootSignature::Initialize(ID3D12Device* pDevice)
	{
		
		//b0 (t0) registerspace for a ConstantBuffer
		D3D12_ROOT_PARAMETER rootParameter[5];
		//pass data
		rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameter[0].Descriptor.ShaderRegister = 0;
		rootParameter[0].Descriptor.RegisterSpace = 0;
		rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		//transform
		rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameter[1].Descriptor.ShaderRegister = 1;
		rootParameter[1].Descriptor.RegisterSpace = 0;
		rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

		//material
		rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameter[2].Descriptor.ShaderRegister = 2;
		rootParameter[2].Descriptor.RegisterSpace = 0;
		rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


		D3D12_DESCRIPTOR_RANGE range[1];
		range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		range[0].BaseShaderRegister = 0;
		range[0].RegisterSpace = 0;
		range[0].NumDescriptors = 5;
		range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	
		rootParameter[3].DescriptorTable.pDescriptorRanges = &range[0];
		rootParameter[3].DescriptorTable.NumDescriptorRanges = 1;

		rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		rootParameter[4].Constants.Num32BitValues = 2;
		rootParameter[4].Constants.ShaderRegister = 0;
		rootParameter[4].Constants.RegisterSpace = 1;
		rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


		D3D12_STATIC_SAMPLER_DESC samplerDesc[3];
		/* static samplers */
		{
			samplerDesc[0].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
			samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			samplerDesc[0].ShaderRegister = 0;
			samplerDesc[0].RegisterSpace = 1;

			samplerDesc[0].MinLOD = 0;
			samplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
			samplerDesc[0].MipLODBias = 0.0f;
			samplerDesc[0].MaxAnisotropy = 16;
			samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
			samplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
			samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;



			samplerDesc[1].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
			samplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			samplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			samplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			samplerDesc[1].ShaderRegister = 0;
			samplerDesc[1].RegisterSpace = 0;
		
			samplerDesc[1].MinLOD = 0;
			samplerDesc[1].MaxLOD = D3D12_FLOAT32_MAX;
			samplerDesc[1].MipLODBias = 0.0f;
			samplerDesc[1].MaxAnisotropy = 1;
			samplerDesc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
			samplerDesc[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;




			samplerDesc[2].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			samplerDesc[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			samplerDesc[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			samplerDesc[2].ShaderRegister = 1;
			samplerDesc[2].RegisterSpace = 0;


			samplerDesc[2].MinLOD = 0;
			samplerDesc[2].MaxLOD = D3D12_FLOAT32_MAX;
			samplerDesc[2].MipLODBias = 0.0f;
			samplerDesc[2].MaxAnisotropy = 1;
			samplerDesc[2].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
			samplerDesc[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


		}



		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rsDesc = { };
		rsDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;
		rsDesc.Desc_1_0.pParameters = rootParameter;
		rsDesc.Desc_1_0.NumParameters = 5; 
		rsDesc.Desc_1_0.NumStaticSamplers = 3;
		rsDesc.Desc_1_0.pStaticSamplers = samplerDesc;
		rsDesc.Desc_1_0.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ID3DBlob* pSerializedRS = nullptr;
		ID3DBlob* pErrorBlob = nullptr;

		YT_EVAL_HR(D3D12SerializeVersionedRootSignature(&rsDesc, &pSerializedRS, &pErrorBlob), "Error serializing the root signature");

		if (pErrorBlob) {

			PRINT_N("Rootsignature serialization error: " << (const char*)pErrorBlob->GetBufferPointer());
		}

		YT_EVAL_HR(pDevice->CreateRootSignature(0, pSerializedRS->GetBufferPointer(), pSerializedRS->GetBufferSize(), IID_PPV_ARGS(GetAddressOf())), "Error creating the root signature");


	}
	void D12RootSignature::Release()
	{
		if (Get()) {

			Reset();
		}
	}
}