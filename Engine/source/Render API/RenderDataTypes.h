#pragma once
#include "pch.h"


namespace Engine {


	namespace Render {

		struct Vertex {
			DirectX::XMFLOAT3 position = { 0.0f,0.0f,0.0f };
			DirectX::XMFLOAT4 color = { 0.0f,1.0f,0.0f,1.0f };

		};


		struct PassData {
			DirectX::XMMATRIX viewproject = DirectX::XMMatrixIdentity();
		};


	}

}