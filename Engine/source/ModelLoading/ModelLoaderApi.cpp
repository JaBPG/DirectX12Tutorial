#include "pch.h"
#include "ModelLoaderApi.h"

#include <fbxsdk.h>


namespace Engine {


	FbxVector4 GetNormal(FbxGeometryElementNormal* normal, const UINT32 faceidx) {
		if (normal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
			if (normal->GetReferenceMode() == FbxGeometryElement::eDirect) {
				return normal->GetDirectArray().GetAt(faceidx);
			}
			const int idx = normal->GetIndexArray().GetAt(faceidx);
			return normal->GetDirectArray().GetAt(idx);

		}
		else {
			PRINT_N("Not implemented normal handling for other than face indices");
		}

		return FbxVector4();
	}

	void ModelLoaderApi::LoadFBXModel(const char* filepath, std::vector<Render::Vertex>& outVertices, std::vector<UINT32>& outIndices)
	{

		FbxManager* manager = FbxManager::Create();

		FbxIOSettings* iosettings = FbxIOSettings::Create(manager, IOSROOT);
		manager->SetIOSettings(iosettings);

		FbxImporter* importer = FbxImporter::Create(manager, "FBX importer");

		if (!importer->Initialize(filepath, -1, manager->GetIOSettings())) {
			PRINT_N("Model loading failed");
			PRINT_N("Error code was: " << importer->GetStatus().GetErrorString());
		}
		else {
			PRINT_N("SUCCES: LOADED MODEL AT " << filepath);

		}

		FbxScene* scene = FbxScene::Create(manager, "ModelLoadingScene");

		importer->Import(scene);

		importer->Destroy();


		unsigned int totalvertices = 0;
		unsigned int totalindices = 0;

		for (int geometry = 0; geometry < scene->GetGeometryCount(); geometry++) {

			PRINT_N("Geometry #" << geometry+1);

			FbxMesh* mesh = static_cast<FbxMesh*>(scene->GetGeometry(geometry));



			std::vector<DirectX::XMFLOAT3> verticespositions;
			verticespositions.reserve(mesh->GetControlPointsCount());

			std::vector<UINT32> indices;
			std::vector<UINT32> normalindcies;

			std::vector<DirectX::XMFLOAT3> polygonnormals;

			/* Create the vertices */
			for (int i = 0; i < mesh->GetControlPointsCount(); i++) {
				const FbxVector4& position = mesh->GetControlPointAt(i);

				//Render::Vertex vertex;
				DirectX::XMFLOAT3 positiondx = { (float)position[0],(float)position[1],(float)position[2] };
				verticespositions.emplace_back(positiondx);
			}

			
			/* Create indices and normals */
			for (int polygon = 0; polygon < mesh->GetPolygonCount(); polygon++) {
				const int polygonsize = mesh->GetPolygonSize(polygon);

				for (int polyvert = 0; polyvert < polygonsize; polyvert++) {

					UINT32 idx = mesh->GetPolygonVertex(polygon, polyvert);
				
					FbxGeometryElementNormal* ntest = mesh->GetElementNormal();
					ntest->RemapIndexTo(FbxLayerElement::eByControlPoint);


					indices.emplace_back(idx);

					totalindices++;

					//const FbxVector4& normalvec4 = GetNormal(mesh->GetElementNormal(),idx);
					const FbxVector4& normalvec4 = GetNormal(ntest,idx);

					DirectX::XMFLOAT3 normal = { (float)normalvec4[0],(float)normalvec4[1],(float)normalvec4[2] };

					polygonnormals.emplace_back(normal);
				}
				

			}
			
			PRINT_N("Loaded model!");
			
		}

	}

}