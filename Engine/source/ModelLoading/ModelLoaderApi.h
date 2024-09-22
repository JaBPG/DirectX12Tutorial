#pragma once
#include "pch.h"
#include "../Render API/RenderDataTypes.h"


namespace Engine {
	YT_API class ModelLoaderApi
	{
	public:
		ModelLoaderApi() = default;


		void LoadFBXModel(const char* filepath, std::vector<Render::Vertex>& outVertices, std::vector<UINT32>& outIndices);

	};


}
