#pragma once
#include <wrl.h>

#include "D12RootSignature.h"
#include "HLSLShader.h"

namespace Engine {
	class YT_API D12PipelineState : public Microsoft::WRL::ComPtr<ID3D12PipelineState>
	{
	public:
		D12PipelineState() = default;
		~D12PipelineState();

		void Initialize(ID3D12Device* pDevice);

		void Release(); 

		inline ID3D12RootSignature* GetRS() { return mRootSignature.Get(); }

	private:
		D12RootSignature mRootSignature;
		HLSLShader mShaders[2];


	};


}

