#pragma once

namespace Engine {

	

	class HLSLShader
	{
	public:
		enum ShaderType {
			VERTEX,
			PIXEL,


		};
	public:

		HLSLShader() = default;
		~HLSLShader();
		void Initialize(const LPCWSTR filename, const ShaderType shaderType);

		void Release();
		
		inline ID3DBlob* GetByteBlob() { return mByteCode; }


	private:

		ID3DBlob* mByteCode = nullptr;
	};
}


