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

	bool CompareVertex(Render::Vertex& a, Render::Vertex& b) {
		if (a.position.x == b.position.x && a.position.y == b.position.y && a.position.z == b.position.z) {
			if (a.normal.x == b.normal.x && a.normal.y == b.normal.y && a.normal.z == b.normal.z) {

				return true;
			}

		}
		return false;
	}

	void ModelLoaderApi::LoadFBXModel(const char* filepath, std::vector<Render::Vertex>& outVertices, std::vector<UINT32>& outIndices, std::vector<Render::MeshDrawData>& outMeshes)
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


		std::vector<Render::Vertex> totalvertices;
		std::vector<UINT32> totalindices;

		size_t vertexoffset = 0;
		size_t indexoffset = 0;


		for (int geometry = 0; geometry < scene->GetGeometryCount(); geometry++) {

			PRINT_N("Geometry #" << geometry+1);

			FbxMesh* mesh = static_cast<FbxMesh*>(scene->GetGeometry(geometry));


			std::vector<Render::Vertex> meshvertices;
			std::vector<UINT32> meshindices;

			const int facecount = mesh->GetPolygonCount();
			FbxVector4* controlpoints = mesh->GetControlPoints();
			const int contronpointcount = mesh->GetControlPointsCount();

			unsigned int vtx = 0;

			for (int face = 0; face < facecount; face++) {
				const int facervertexcount = mesh->GetPolygonSize(face);

				for (int facevtx = 0; facevtx < facervertexcount; facevtx++) {

					const int controlpointidx = mesh->GetPolygonVertex(face, facevtx);

					Render::Vertex vertex;
					const FbxVector4& positondata = controlpoints[controlpointidx];

					vertex.position = { (float)positondata[0],(float)positondata[1],(float)positondata[2] };


					const int normalcount = mesh->GetElementNormalCount();

					for (int normalelem = 0; normalelem < normalcount; normalelem++) {
						FbxGeometryElementNormal* pnormal = mesh->GetElementNormal(normalelem);

						const FbxVector4& normaldata = GetNormal(pnormal, vtx);

						vertex.normal = { (float)normaldata[0],(float)normaldata[1],(float)normaldata[2] };
					}


					const size_t meshvtxcnt = meshvertices.size();
					size_t i = 0;
					for (i = 0; i < meshvtxcnt; i++) {
						if (CompareVertex(vertex, meshvertices[i])) {
							break;
						}
					}

					if (i == meshvtxcnt) {

						meshvertices.emplace_back(vertex);
					}

					meshindices.push_back(i);
					vtx++;
				}
			
			}

			Render::MeshDrawData meshdrawdata;

			meshdrawdata.vertexoffset = vertexoffset;
			meshdrawdata.vertexcount = meshvertices.size();
			meshdrawdata.indexoffset = indexoffset;
			meshdrawdata.indexcount = meshindices.size();
		
			outMeshes.emplace_back(meshdrawdata);


			vertexoffset += meshdrawdata.vertexcount;
			indexoffset += meshdrawdata.indexcount;

			totalvertices.insert(totalvertices.end(), meshvertices.begin(), meshvertices.end());
			totalindices.insert(totalindices.end(), meshindices.begin(), meshindices.end());

			PRINT_N("Loaded model!");
			
		}

		outVertices = std::move(totalvertices);
		outIndices = std::move(totalindices);

	}

}




/* Create the vertices */

/*
for (int i = 0; i < mesh->GetControlPointsCount(); i++) {
	const FbxVector4& position = mesh->GetControlPointAt(i);

	//Render::Vertex vertex;
	DirectX::XMFLOAT3 positiondx = { (float)position[0],(float)position[1],(float)position[2] };
	verticespositions.emplace_back(positiondx);
}

*/
/* Create indices and normals */
/*
for (int polygon = 0; polygon < mesh->GetPolygonCount(); polygon++) {
	const int polygonsize = mesh->GetPolygonSize(polygon);

	for (int polyvert = 0; polyvert < polygonsize; polyvert++) {

		UINT32 idx = mesh->GetPolygonVertex(polygon, polyvert);

		FbxGeometryElementNormal* ntest = mesh->GetElementNormal();

		const auto data = ntest->GetDirectArray();

		const auto mapping = ntest->GetMappingMode();
		const auto refmode = ntest->GetReferenceMode();

		//ntest->RemapIndexTo(FbxLayerElement::eByControlPoint);


		indices.emplace_back(idx);

		totalindices++;

		const FbxVector4& normalvec4 = GetNormal(mesh->GetElementNormal(),idx);
		//const FbxVector4& normalvec4 = GetNormal(ntest,idx);

		DirectX::XMFLOAT3 normal = { (float)normalvec4[0],(float)normalvec4[1],(float)normalvec4[2] };

		polygonnormals.emplace_back(normal);
	}


}

*/