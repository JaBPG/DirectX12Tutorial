#pragma once
#include "pch.h"


namespace Engine {


	namespace Render {

		struct Light {
			DirectX::XMFLOAT3 position = { 0.0f,0.0f,0.0f };
			float strength = 0.0f;
			DirectX::XMFLOAT3 direction = { 0.0f,0.0f,0.0f };
			float padding = 0.0;
		};



		struct Vertex {
			DirectX::XMFLOAT3 position = { 0.0f,0.0f,0.0f };
			DirectX::XMFLOAT3 normal = { 0.0f,0.0f,0.0f };
			//DirectX::XMFLOAT4 color = { 0.0f,1.0f,0.0f,1.0f };

		};


		struct MaterialCelShader {
			DirectX::XMFLOAT4 diffuseAlbedo = { 0.0f,0.0f,0.0f,1.0f };
		};


		struct ObjectData {
			DirectX::XMMATRIX transform = DirectX::XMMatrixIdentity();
		};

		struct PassData {
			DirectX::XMMATRIX viewproject = DirectX::XMMatrixIdentity();
			Light scenelight;
		};


	}

}