////////////////////////////////////////////////////////////////////////////////
// Filename: Mesh.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Mesh.h"
#include "Configuration.h"
#include "DataManager.h"
#include "Environment.h"
#include "Gizmo.h"
#include "Shader.h"
#include "Transform.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


#define RENDER_SINGLE_MESH\
	glBindVertexArray(m_VAO);\
	for(unsigned int i = 0; i<m_Entries.size(); i++) {\
		if(materials) {\
			const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;\
			if((*materials)[MaterialIndex]) (*materials)[MaterialIndex]->BindMaterial();\
																																				}\
		glDrawElementsBaseVertex(drawMode,\
			m_Entries[i].NumIndices,\
			GL_UNSIGNED_INT,\
			(void*) (sizeof(unsigned int) * m_Entries[i].BaseIndex),\
			m_Entries[i].BaseVertex);\
																						}\
	glBindVertexArray(0);

#define RENDER_INSTANCED_MESH\
	glBindVertexArray(m_VAO);\
	for(unsigned int i = 0; i<m_Entries.size(); i++) {\
		if(material) {\
			const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;\
			if((*material)[MaterialIndex]) (*material)[MaterialIndex]->BindMaterial();\
																																		}\
		glDrawElementsInstancedBaseVertex(drawMode,\
			m_Entries[i].NumIndices,\
			GL_UNSIGNED_INT,\
			(void*) (sizeof(unsigned int) * m_Entries[i].BaseIndex),\
			m_MMatrices.size(),\
			m_Entries[i].BaseVertex);\
																	}\
	glBindVertexArray(0);

Mesh::Mesh(const eDataType& dataType) {
	m_MeshType = eMeshType::BASE_MESH;
	m_Scene = nullptr;
	m_VAO = 0;
	m_BoundingBox = nullptr;
	m_NumBones = 0;
	m_DataType = dataType;
}

Mesh::~Mesh() {
	Release();
	m_Scene = nullptr;
	m_BoundingBox = nullptr;
}

void Mesh::Release() {
	// Release all materials.
	for(unsigned int i = 0; i<m_Materials.size(); i++) {
		m_Materials[i] = nullptr; // Set to NULL, materials are released by DataManager class.
	}
	m_Materials.clear();

	// Deleting buffers.
	for each (auto buffer in m_Buffers) {
		DELETE_BUFFER(buffer);
	}
	m_Buffers.clear();

	// Deleting VAO.
	DELETE_VERTEX_ARRAY(m_VAO);

	// Clear all vectors.
	m_Positions.clear();
	m_Normals.clear();
	m_TexCoords.clear();
	m_Tangents.clear();
	m_Indices.clear();
	m_MvpMatrices.clear();
	m_MMatrices.clear();

	FREE_PTR(m_BoundingBox);
}

bool Mesh::CreateMesh() {
	m_Entries.resize(1);
	m_Entries[0].MaterialIndex = 0;
	m_Entries[0].NumIndices = m_Indices.size();
	m_Entries[0].BaseVertex = 0;
	m_Entries[0].BaseIndex = 0;

	m_Materials.resize(1);
	m_Materials[0] = nullptr;

	// Create the VAO.
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	InitializeBuffers();

	// Make sure the VAO is not changed from the outside.
	glBindVertexArray(0);

	return dg::CheckGLError();
}

bool Mesh::CreateMesh(const vector<vec3>& positions, const vector<vec3>& normals, const vector<vec2>& texCoords, const vector<vec3>& tangents, const vector<unsigned int>& indices) {
	m_Positions = std::move(positions);
	m_Normals = std::move(normals);
	m_TexCoords = std::move(texCoords);
	m_Tangents = std::move(tangents);
	m_Indices = std::move(indices);

	m_Entries.resize(1);
	m_Entries[0].MaterialIndex = 0;
	m_Entries[0].NumIndices = m_Indices.size();
	m_Entries[0].BaseVertex = 0;
	m_Entries[0].BaseIndex = 0;

	m_Materials.resize(1);
	m_Materials[0] = nullptr;

	// Create the VAO.
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	InitializeBuffers();

	// Make sure the VAO is not changed from the outside.
	glBindVertexArray(0);

	return dg::CheckGLError();
}

bool Mesh::LoadMesh(const string& filePath, FlagData* flagData) {
	FlagData defaultFlagData;

	// Release the previously loaded mesh (if it exists).
	Release();

	// Create the VAO.
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	if(!flagData) {
		flagData = &defaultFlagData;
	}

	bool Ret = false;

	unsigned int processFlags = 0;
	processFlags |= (flagData->CalcTangentSpace ? aiProcess_CalcTangentSpace : 0);
	processFlags |= (flagData->FindInstances ? aiProcess_FindInstances : 0);
	processFlags |= (flagData->FindInvalidData ? aiProcess_FindInvalidData : 0);
	processFlags |= (flagData->FlipUVs ? aiProcess_FlipUVs : 0);
	processFlags |= (flagData->GenSmoothNormals ? aiProcess_GenSmoothNormals : 0);
	processFlags |= (flagData->GenUVCoords ? aiProcess_GenUVCoords : 0);
	processFlags |= (flagData->ImproveCacheLocality ? aiProcess_ImproveCacheLocality : 0);
	processFlags |= (flagData->JoinIdenticalVertices ? aiProcess_JoinIdenticalVertices : 0);
	processFlags |= (flagData->LimitBoneWeights ? aiProcess_LimitBoneWeights : 0);
	processFlags |= (flagData->OptimizeMeshes ? aiProcess_OptimizeMeshes : 0);
	processFlags |= (flagData->OptimizeGraph ? aiProcess_OptimizeGraph : 0);
	processFlags |= (flagData->PreTransformVertices ? aiProcess_PreTransformVertices : 0);
	processFlags |= (flagData->RemoveRedundantMaterials ? aiProcess_RemoveRedundantMaterials : 0);
	processFlags |= (flagData->SplitByBoneCount ? aiProcess_SplitByBoneCount : 0);
	processFlags |= (flagData->Triangulate ? aiProcess_Triangulate : 0);
	processFlags |= (flagData->TransformUVCoords ? aiProcess_TransformUVCoords : 0);
	processFlags |= (flagData->ValidateDataStructure ? aiProcess_ValidateDataStructure : 0);

	m_Scene = m_Importer.ReadFile(filePath.c_str(), processFlags);

	m_MeshType = eMeshType::BASE_MESH;
	if(m_Scene->HasAnimations()) {
		m_MeshType = eMeshType::SKINNED_MESH;
	}

	if(m_Scene) {
		dg::DebugConsoleMessage("Loading ");
		dg::DebugConsoleMessage(filePath.c_str());
		dg::DebugConsoleMessage("\n");
		Ret = InitializeFromScene(m_Scene, filePath);
	} else {
		dg::DebugConsoleMessage("Error parsing ");
		dg::DebugConsoleMessage(filePath.c_str());
		dg::DebugConsoleMessage(": ");
		dg::DebugConsoleMessage(m_Importer.GetErrorString());
		dg::DebugConsoleMessage("\n");
	}

	// Make sure the VAO is not changed from the outside.
	glBindVertexArray(0);

	return Ret;
}

bool Mesh::CreateBoundingBox() {
	if(m_BoundingBox!=nullptr)
		return true;

	vec3 minExt = GetMinExtension();
	vec3 maxExt = GetMaxExtension();

	return CreateBoundingBox(minExt, maxExt);
}

bool Mesh::CreateBoundingBox(vec3 minExt, vec3 maxExt) {
	if(m_BoundingBox!=nullptr)
		return true;

	m_BoundingBox = new Mesh();

	vector<vec3> positions;
	vector<vec3> normals;
	vector<vec2> texCoords;
	vector<vec3> tangents;
	vector<unsigned int> indices;

	vec3 diff;

	for(unsigned i = 0; i<3; i++) {
		diff[i] = maxExt[i]-minExt[i];
	}

	unsigned int numVertices = 8;
	unsigned int numIndices = 36;
	positions.reserve(numVertices);
	normals.reserve(numVertices);
	texCoords.reserve(numVertices);
	tangents.reserve(numVertices);
	indices.reserve(numIndices);

	// V0
	positions.push_back(minExt);
	texCoords.push_back(vec2(0.0f, 0.0f));
	normals.push_back(vec_up);
	tangents.push_back(vec_zero);
	// V1
	positions.push_back(minExt+(vec_right) *diff);
	texCoords.push_back(vec2(0.0f, 0.0f));
	normals.push_back(vec_up);
	tangents.push_back(vec_zero);
	// V2
	positions.push_back(minExt+(vec_right+vec_up)*diff);
	texCoords.push_back(vec2(0.0f, 0.0f));
	normals.push_back(vec_up);
	tangents.push_back(vec_zero);
	// V3
	positions.push_back(minExt+(vec_up) *diff);
	texCoords.push_back(vec2(0.0f, 0.0f));
	normals.push_back(vec_up);
	tangents.push_back(vec_zero);
	// V4
	positions.push_back(minExt+(vec_forward) *diff);
	texCoords.push_back(vec2(0.0f, 0.0f));
	normals.push_back(vec_up);
	tangents.push_back(vec_zero);
	// V5
	positions.push_back(minExt+(vec_forward+vec_right) *diff);
	texCoords.push_back(vec2(0.0f, 0.0f));
	normals.push_back(vec_up);
	tangents.push_back(vec_zero);
	// V6
	positions.push_back(minExt+(vec_forward+vec_right+vec_up)*diff);
	texCoords.push_back(vec2(0.0f, 0.0f));
	normals.push_back(vec_up);
	tangents.push_back(vec_zero);
	// V7
	positions.push_back(minExt+(vec_forward+vec_up) *diff);
	texCoords.push_back(vec2(0.0f, 0.0f));
	normals.push_back(vec_up);
	tangents.push_back(vec_zero);

#define NUM_INDEX 24

	unsigned int index[NUM_INDEX] = {
		// Front
		3, 2, 1, 0,
		// Right
		2, 6, 5, 1,
		// Back
		6, 7, 4, 5,
		// Left
		7, 3, 0, 4,
		// Bottom
		0, 1, 5, 4,
		// Top
		7, 6, 2, 3
	};

	for(unsigned int i = 0; i<NUM_INDEX; i++) {
		indices.push_back(index[i]);
	}

#undef NUM_INDEX

	if(!m_BoundingBox->CreateMesh(positions, normals, texCoords, tangents, indices)) {
		FREE_PTR(m_BoundingBox);
		m_BoundingBox = nullptr;
	}

	positions.clear();
	normals.clear();
	texCoords.clear();
	tangents.clear();
	indices.clear();

	dg::CheckGLError();

	return (m_BoundingBox!=nullptr);
}

bool Mesh::InitializeFromScene(const aiScene* pScene, const string& fileName) {
	m_Entries.resize(pScene->mNumMeshes);

	unsigned int NumVertices = 0;
	unsigned int NumIndices = 0;

	// Count the number of vertices and indices
	for(unsigned int i = 0; i<m_Entries.size(); i++) {
		m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
		m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces*3;
		m_Entries[i].BaseVertex = NumVertices;
		m_Entries[i].BaseIndex = NumIndices;

		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumIndices += m_Entries[i].NumIndices;
	}

	// Reserve space in the vectors for the vertex attributes and indices
	m_Positions.reserve(NumVertices);
	m_Normals.reserve(NumVertices);
	m_TexCoords.reserve(NumVertices);
	m_Tangents.reserve(NumVertices);
	m_Indices.reserve(NumIndices);

	if(m_MeshType==eMeshType::SKINNED_MESH) {
		m_Bones.resize(NumVertices);
	}

	// Initialize the meshes in the scene one by one
	for(unsigned int i = 0; i<m_Entries.size(); i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitializeMesh(i, paiMesh);
	}

	if(!InitializeMaterials(pScene, fileName)) {
		return false;
	}

	InitializeBuffers();

	return dg::CheckGLError();
}

void Mesh::InitializeBuffers() {
	// Generate and populate the buffers with vertex attributes and the indices.
	m_Buffers.resize(eVertexBuffer::NUM_VBs);
	for(unsigned int i = 0; i<eVertexBuffer::NUM_VBs; i++) {
		glGenBuffers(1, &m_Buffers[i]);
	}

	GLenum bufferType = m_DataType==eDataType::STATIC_DATA ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;

	// Positions.
	if(m_Positions.size()>0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eVertexBuffer::POS_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * m_Positions.size(), glm::value_ptr(m_Positions.front()), bufferType);
		glEnableVertexAttribArray(POSITION_LOCATION);
		glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	// Texture coordinates.
	if(m_TexCoords.size()>0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eVertexBuffer::TEXCOORD_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * m_TexCoords.size(), glm::value_ptr(m_TexCoords.front()), bufferType);
		glEnableVertexAttribArray(TEX_COORD_LOCATION);
		glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}

	// Normals.
	if(m_Normals.size()>0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eVertexBuffer::NORMAL_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * m_Normals.size(), glm::value_ptr(m_Normals.front()), bufferType);
		glEnableVertexAttribArray(NORMAL_LOCATION);
		glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	// Tangents.
	if(m_Tangents.size()>0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eVertexBuffer::TANGENT_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * m_Tangents.size(), glm::value_ptr(m_Tangents.front()), bufferType);
		glEnableVertexAttribArray(TANGENT_LOCATION);
		glVertexAttribPointer(TANGENT_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	// Model view projection matrix for instancing drawing.
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eVertexBuffer::MODEL_VIEW_PROJECTION_MATRIX_VB]);
	for(unsigned int i = 0; i<4; i++) {
		glEnableVertexAttribArray(WVP_LOCATION+i);
		glVertexAttribPointer(WVP_LOCATION+i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (const GLvoid*) (sizeof(GLfloat) * i*4));
		glVertexAttribDivisor(WVP_LOCATION+i, 1);
	}

	// Model matrix for instancing drawing.
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eVertexBuffer::MODEL_MATRIX_VB]);
	for(unsigned int i = 0; i<4; i++) {
		glEnableVertexAttribArray(WORLD_LOCATION+i);
		glVertexAttribPointer(WORLD_LOCATION+i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (const GLvoid*) (sizeof(GLfloat) * i*4));
		glVertexAttribDivisor(WORLD_LOCATION+i, 1);
	}

	// Indices
	if(m_Indices.size()>0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[eVertexBuffer::INDEX_VB]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_Indices.size(), &m_Indices.front(), bufferType);
	}

	// Bones.
	if(m_MeshType==eMeshType::SKINNED_MESH && m_Bones.size()>0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eVertexBuffer::BONE_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexBoneData) * m_Bones.size(), &m_Bones.front(), bufferType);
		glEnableVertexAttribArray(BONE_ID_LOCATION);
		glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*) 0);
		glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
		glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*) 16);
	}

	// Avoid problems with instaced buffers.
	mat4 matrix;
	vector<mat4> mvpMatrices;
	mvpMatrices.push_back(matrix);
	vector<mat4> worldMatrices;
	worldMatrices.push_back(matrix);
	SetMVPMatrices(mvpMatrices);
	SetMMatrices(worldMatrices);
	UpdateMVPMatrices();
	UpdateMMatrices();
	glBindVertexArray(m_VAO);
}

void Mesh::InitializeMesh(unsigned int MeshIndex, const aiMesh* paiMesh) {
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
	const aiVector3D Up3D(0.0f, 1.0f, 0.0f);

	// Populate the vertex attribute vectors.
	for(unsigned int i = 0; i<paiMesh->mNumVertices; i++) {
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = paiMesh->HasNormals() ? &(paiMesh->mNormals[i]) : &Up3D;
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
		const aiVector3D* pTangent = paiMesh->HasTangentsAndBitangents() ? &(paiMesh->mTangents[i]) : &Zero3D;

		m_Positions.push_back(vec3(pPos->x, pPos->y, pPos->z));
		m_Normals.push_back(normalize(vec3(pNormal->x, pNormal->y, pNormal->z)));
		m_TexCoords.push_back(vec2(pTexCoord->x, pTexCoord->y));
		m_Tangents.push_back(vec3(pTangent->x, pTangent->y, pTangent->z));
	}

	if(m_MeshType==eMeshType::SKINNED_MESH) {
		LoadBones(MeshIndex, paiMesh);
	}

	// Populate the index buffer.
	for(unsigned int i = 0; i<paiMesh->mNumFaces; i++) {
		const aiFace& Face = paiMesh->mFaces[i];
		assert(Face.mNumIndices==3);
		m_Indices.push_back(Face.mIndices[0]);
		m_Indices.push_back(Face.mIndices[1]);
		m_Indices.push_back(Face.mIndices[2]);
	}
}

bool Mesh::InitializeMaterials(const aiScene* pScene, const string& fileName) {
	m_Materials.resize(pScene->mNumMaterials);

	// Extract the directory part from the file name.
	string::size_type SlashIndex = fileName.find_last_of("/");
	string Dir;

	if(SlashIndex==string::npos) {
		Dir = ".";
	} else if(SlashIndex==0) {
		Dir = "/";
	} else {
		Dir = fileName.substr(0, SlashIndex);
	}

	// Initialize the materials.
	for(unsigned int i = 0; i<pScene->mNumMaterials; i++) {
		MaterialDesc materialDesc;

		// Material name.
		aiString aiMaterialName;
		pScene->mMaterials[i]->Get(AI_MATKEY_NAME, aiMaterialName);
		materialDesc.materialName = aiMaterialName.C_Str();

		// Set all material properties.
		if(m_MeshType==eMeshType::SKINNED_MESH) {
			materialDesc.shaderName = SHADER_SKINNED_MESH;
		} else {
			materialDesc.shaderName = SHADER_BASE_MESH;
		}

		pScene->mMaterials[i]->Get(AI_MATKEY_ENABLE_WIREFRAME, materialDesc.wireframe);
		pScene->mMaterials[i]->Get(AI_MATKEY_OPACITY, materialDesc.opacity);
		pScene->mMaterials[i]->Get(AI_MATKEY_OPACITY, materialDesc.bumpScaling);
		pScene->mMaterials[i]->Get(AI_MATKEY_SHININESS, materialDesc.shininess);
		pScene->mMaterials[i]->Get(AI_MATKEY_SHININESS_STRENGTH, materialDesc.shininessStrength);
		pScene->mMaterials[i]->Get(AI_MATKEY_REFLECTIVITY, materialDesc.reflectivity);
		pScene->mMaterials[i]->Get(AI_MATKEY_REFRACTI, materialDesc.refraction);

		aiColor4D ai_color;

		pScene->mMaterials[i]->Get(AI_MATKEY_COLOR_DIFFUSE, ai_color);
		materialDesc.colorDiffuse = vec4(ai_color.r, ai_color.g, ai_color.b, ai_color.a);

		pScene->mMaterials[i]->Get(AI_MATKEY_COLOR_AMBIENT, ai_color);
		materialDesc.colorAmbient = vec4(ai_color.r, ai_color.g, ai_color.b, ai_color.a);

		pScene->mMaterials[i]->Get(AI_MATKEY_COLOR_SPECULAR, ai_color);
		materialDesc.colorSpecular = vec4(ai_color.r, ai_color.g, ai_color.b, ai_color.a);

		pScene->mMaterials[i]->Get(AI_MATKEY_COLOR_EMISSIVE, ai_color);
		materialDesc.colorEmissive = vec4(ai_color.r, ai_color.g, ai_color.b, ai_color.a);

		// Textures.
		for(unsigned int j = 0; j<eTextureType::NUM_TEXTUREs; j++) {
			if(pScene->mMaterials[i]->GetTextureCount(aiTextureType(j))>0) {
				aiString Path;
				if(pScene->mMaterials[i]->GetTexture(aiTextureType(j), 0, &Path, NULL, NULL, NULL, NULL, NULL)==AI_SUCCESS) {
					std::string texturePath = Dir+"/"+Path.data;
					//materialDesc.texturesPath[j] = texturePath;

					bool mipMap =
						eTextureType(j)==eTextureType::DIFFUSE_TEXTURE||
						eTextureType(j)==eTextureType::NORMAL_TEXTURE;
					materialDesc.textureDesc[j].filePath = texturePath;
					materialDesc.textureDesc[j].mipMap = mipMap;
				}
			}
		}

		m_Materials[i] = DataManager::TryGetMaterial(materialDesc);
	}

	return true;
}

void Mesh::LoadBones(unsigned int MeshIndex, const aiMesh* pMesh) {
	for(unsigned int i = 0; i<pMesh->mNumBones; i++) {
		unsigned int BoneIndex = 0;
		string BoneName(pMesh->mBones[i]->mName.data);

		if(m_BoneMapping.find(BoneName)==m_BoneMapping.end()) {
			// Allocate an index for a new bone
			BoneIndex = m_NumBones;
			m_NumBones++;
			BoneInfo bi;
			m_BoneInfo.push_back(bi);
			//m_BoneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;
			memcpy(glm::value_ptr(m_BoneInfo[BoneIndex].BoneOffset), &pMesh->mBones[i]->mOffsetMatrix, sizeof(float)*16);
			m_BoneMapping[BoneName] = BoneIndex;
		} else {
			BoneIndex = m_BoneMapping[BoneName];
		}

		for(unsigned int j = 0; j<pMesh->mBones[i]->mNumWeights; j++) {
			unsigned int VertexID = m_Entries[MeshIndex].BaseVertex+pMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			m_Bones[VertexID].addBoneData(BoneIndex, Weight);
		}
	}
}

void Mesh::Render(const GLuint& query, const vector<shared_ptr<Material>>* materials, GLenum drawMode) const {
	if(m_BoundingBox && query!=NULL_QUERY && Environment::Instance()->IsOcclusionRenderingActive()) {
		//GLuint the_result = 0;
		//int count = 1000;
		//while(the_result==0 && count--) {
		//	glGetQueryObjectuiv(m_Query, GL_QUERY_RESULT_AVAILABLE, &the_result);
		//	if(the_result!=0) {
		//		glGetQueryObjectuiv(m_Query, GL_QUERY_RESULT, &the_result);
		//		dg::DebugConsoleMessage("Samples: ");
		//		dg::DebugConsoleMessage(std::to_string(the_result).c_str());
		//		dg::DebugConsoleMessage("\n");
		//	} 
		//}
		glBeginConditionalRender(query, GL_QUERY_NO_WAIT);
		RENDER_SINGLE_MESH;
		glEndConditionalRender();
	} else {
		RENDER_SINGLE_MESH;
	}

	dg::CheckGLError();
}

void Mesh::RenderInstanced(const GLuint& query, const vector<shared_ptr<Material>>* material, GLenum drawMode) const {
	if(m_BoundingBox && query!=NULL_QUERY &&Environment::Instance()->IsOcclusionRenderingActive()) {
		glBeginConditionalRender(query, GL_QUERY_NO_WAIT);
		RENDER_INSTANCED_MESH;
		glEndConditionalRender();
	} else {
		RENDER_INSTANCED_MESH;
	}

	dg::CheckGLError();
}

void Mesh::RenderBoundingBox(Transform* transform) const {
	if(m_BoundingBox) {
		GLint cullFace;
		glGetIntegerv(GL_CULL_FACE, &cullFace);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);		// Wireframe
		glDisable(GL_CULL_FACE);

		shared_ptr<Shader> shaderGizmo = DataManager::GetShader(SHADER_GIZMO);

		shaderGizmo->SetActive();
		shaderGizmo->SetInt("AlbedoMap", COLOR_TEXTURE_UNIT_INDEX);
		shaderGizmo->SetInt("AlbedoMap_bool", false);
		shaderGizmo->SetFloatVector4("Color", color_green);
		shaderGizmo->SetInt("ActiveOIT", Environment::Instance()->IsOITActive());
		shaderGizmo->SetFloatMatrix4("MVPMatrix", transform->GetWorldViewProjectionMatrix());

		m_BoundingBox->Render(NULL_QUERY, NULL, GL_QUADS);

		if(cullFace) glEnable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);		// Shaded
	}
}

void Mesh::OcclusionQuery(const GLuint& query, const vector<mat4>& mvpMatrices) const {
	if(!m_BoundingBox)
		return;

	unsigned int numInstances = mvpMatrices.size();

	// Disable write to framebuffer.
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);

	// Begin query.
	glBeginQuery(GL_SAMPLES_PASSED, query);

	// Do occlusion render.
	glBindVertexArray(m_BoundingBox->m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_BoundingBox->m_Buffers[MODEL_VIEW_PROJECTION_MATRIX_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * mvpMatrices.size(), value_ptr(mvpMatrices[0]), GL_DYNAMIC_DRAW);

	if(numInstances>1) { // Instancing
		for(unsigned int i = 0; i<m_BoundingBox->m_Entries.size(); i++) {
			glDrawElementsInstancedBaseVertex(GL_QUADS,
				m_BoundingBox->m_Entries[i].NumIndices,
				GL_UNSIGNED_INT,
				(void*) (sizeof(unsigned int) * m_BoundingBox->m_Entries[i].BaseIndex),
				numInstances,
				m_BoundingBox->m_Entries[i].BaseVertex);
		}
	} else {
		for(unsigned int i = 0; i<m_BoundingBox->m_Entries.size(); i++) {
			glDrawElementsBaseVertex(GL_QUADS,
				m_BoundingBox->m_Entries[i].NumIndices,
				GL_UNSIGNED_INT,
				(void*) (sizeof(unsigned int) * m_BoundingBox->m_Entries[i].BaseIndex),
				m_BoundingBox->m_Entries[i].BaseVertex);
		}
	}
	glBindVertexArray(0);

	// End query.
	glEndQuery(GL_SAMPLES_PASSED);

	// Enable write to framebuffer.
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);

	dg::CheckGLError();
}

const vector<shared_ptr<Material>>& Mesh::GetDefaultMaterials() const {
	return m_Materials;
}

const vec3 Mesh::GetMaxExtension() const {
	vec3 result(0.0f, 0.0f, 0.0f);
	if(m_Positions.size()>0) {
		result = m_Positions[0];
		for(unsigned int i = 1; i<m_Positions.size(); i++) {
			for(size_t j = 0; j<3; j++) {
				if(m_Positions[i][j]>result[j]) {
					result[j] = m_Positions[i][j];
				}
			}
		}
	}
	return result;
}

const vec3 Mesh::GetMinExtension() const {
	vec3 result(0.0f, 0.0f, 0.0f);
	if(m_Positions.size()>0) {
		result = m_Positions[0];
		for(unsigned int i = 1; i<m_Positions.size(); i++) {
			for(size_t j = 0; j<3; j++) {
				if(m_Positions[i][j]<result[j]) {
					result[j] = m_Positions[i][j];
				}
			}
		}
	}
	return result;
}

bool Mesh::IsStatic() const {
	return m_DataType==eDataType::STATIC_DATA;
}

const eMeshType Mesh::GetType() const {
	return m_MeshType;
}

unsigned int Mesh::NumBones() const {
	return m_NumBones;
}

void Mesh::SetPositions(vector<vec3>& vector) {
	m_Positions = std::move(vector);
}

vector<vec3>& Mesh::GetPositions() {
	return m_Positions;
}

void Mesh::SetNormals(vector<vec3>& vector) {
	m_Normals = std::move(vector);
}

vector<vec3>& Mesh::GetNormals() {
	return m_Normals;
}

void Mesh::SetTexCoords(vector<vec2>& vector) {
	m_TexCoords = std::move(vector);
}

vector<vec2>& Mesh::GetTexCoords() {
	return m_TexCoords;
}

void Mesh::SetTangents(vector<vec3>& vector) {
	m_Tangents = std::move(vector);
}

vector<vec3>& Mesh::GetTangents() {
	return m_Tangents;
}

void Mesh::SetMVPMatrices(vector<mat4>& vector) {
	m_MvpMatrices = std::move(vector);
}

vector<mat4>& Mesh::GetMVPMatrices() {
	return m_MvpMatrices;
}

void Mesh::SetMMatrices(vector<mat4>& vector) {
	m_MMatrices = std::move(vector);
}

vector<mat4>& Mesh::GetMMatrices() {
	return m_MMatrices;
}

void Mesh::SetIndices(vector<unsigned int>& vector) {
	m_Indices = std::move(vector);
}

vector<unsigned int>& Mesh::GetIndices() {
	return m_Indices;
}

void Mesh::UpdatePositions() {
	if(IsStatic())
		return;

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eVertexBuffer::POS_VB]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_Positions.size(), glm::value_ptr(m_Positions.front()));
	glBindVertexArray(0);
}

void Mesh::UpdateTexCoords() {
	if(IsStatic())
		return;

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eVertexBuffer::TEXCOORD_VB]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_TexCoords.size(), glm::value_ptr(m_TexCoords.front()));
	glBindVertexArray(0);
}

void Mesh::UpdateNormals() {
	if(IsStatic())
		return;

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eVertexBuffer::NORMAL_VB]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_Normals.size(), glm::value_ptr(m_Normals.front()));
	glBindVertexArray(0);
}

void Mesh::UpdateTangents() {
	if(IsStatic())
		return;

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eVertexBuffer::TANGENT_VB]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_Tangents.size(), glm::value_ptr(m_Tangents.front()));
	glBindVertexArray(0);
}

void Mesh::UpdateIndices() {
	if(IsStatic())
		return;

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[eVertexBuffer::INDEX_VB]);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * m_Indices.size(), &m_Indices.front());
	glBindVertexArray(0);
}

void Mesh::UpdateMVPMatrices() {
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eVertexBuffer::MODEL_VIEW_PROJECTION_MATRIX_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * m_MvpMatrices.size(), glm::value_ptr(m_MvpMatrices.front()), GL_DYNAMIC_DRAW);
	glBindVertexArray(0);
}

void Mesh::UpdateMMatrices() {
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eVertexBuffer::MODEL_MATRIX_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * m_MMatrices.size(), glm::value_ptr(m_MMatrices.front()), GL_DYNAMIC_DRAW);
	glBindVertexArray(0);
}

void Mesh::UpdateBones() {
	if(IsStatic()||m_MeshType!=eMeshType::SKINNED_MESH)
		return;

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[eVertexBuffer::BONE_VB]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VertexBoneData) * m_Bones.size(), &m_Bones.front());
	glBindVertexArray(0);
}
