////////////////////////////////////////////////////////////////////////////////
// Filename: DataManager.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "DataManager.h"
#include "Mesh.h"
#include "Glossary.h"
#include "Light.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "SceneObject.h"
#include "Shader.h"
#include "ShaderBaseMesh.h"
#include "ShaderBloom.h"
#include "ShaderBlur.h"
#include "ShaderGeometry.h"
#include "ShaderGizmo.h"
#include "ShaderInfoBarRenderer.h"
#include "ShaderLogo.h"
#include "ShaderNormalViewer.h"
#include "ShaderOcclusionRender.h"
#include "ShaderPostProcessing.h"
#include "ShaderShadowMap.h"
#include "ShaderShadowMap2.h"
#include "ShaderShadowCubeMap.h"
#include "ShaderShadowCubeMap2.h"
#include "ShaderSkybox.h"
#include "ShaderSkinnedMesh.h"
#include "ShaderSubstateMeshInstanced.h"
#include "ShaderSubstateMeshVolumeFrontFace.h"
#include "ShaderSubstateMeshVolume.h"
#include "ShaderSubstateMeshSurface.h"
#include "ShaderTextRenderer.h"
#include "ShaderSSAO.h"
#include "Texture.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


static aiScene* scene = nullptr;
unique_ptr<DataManager>	DataManager::m_Instance = nullptr;
SceneObject*			DataManager::m_ActiveSceneObject = nullptr;
GLuint					DataManager::m_DefaultFramebuffer = 0;

const unique_ptr<DataManager>& DataManager::Instance() {
	if(!m_Instance) {
		m_Instance = unique_ptr<DataManager>(new DataManager());
	}
	return m_Instance;
}

DataManager::DataManager() {
	m_MainCamera = nullptr;

	// Get the number of physical cores.
	unsigned int cores = std::thread::hardware_concurrency();
	m_SceneObjectPool.reserve(cores);

	// Fill the pool with empty maps.
	for(unsigned int i = 0; i<cores; i++) {
		m_SceneObjectPool.push_back(IdSceneObjectMap());
	}

	//m_ThreadPool = unique_ptr<ThreadPool>(new ThreadPool());
	m_ThreadPool = nullptr;
}

DataManager::~DataManager() {
	m_ThreadPool = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// SceneObjet.
////////////////////////////////////////////////////////////////////////////////
unsigned int DataManager::GetNumSceneObject() {
	return Instance()->m_IdMap.size();
}

void DataManager::AddSceneObject(SceneObject* sceneObject) {
	Instance()->m_IdMap[sceneObject->Id()] = sceneObject;
	Instance()->m_TagMap[sceneObject->Tag()].push_back(sceneObject);

	// Insert the sceneObject also in the pool.
	AddSceneObjectToPool(sceneObject);
}

void DataManager::RemoveSceneObject(string id) {
	SceneObject* sceneObject = Instance()->m_IdMap[id];

	if(sceneObject) {
		vector<SceneObject*> tagVector = Instance()->m_TagMap[sceneObject->Tag()];
		int index = 0;
		for(vector<SceneObject*>::iterator it = tagVector.begin(); it!=tagVector.end(); it++, index++) {
			if((*it)==sceneObject) {
				break;
			}
		}
		tagVector.erase(tagVector.begin()+index);
	}

	// Remove element from id map.
	Instance()->m_IdMap.erase(id);

	// Remove the sceneObject also from the pool.
	RemoveSceneObjectFromPool(id);
}

SceneObject* DataManager::FindSceneObjectById(string id) {
	return Instance()->m_IdMap[id];
}

vector<SceneObject*>& DataManager::FindSceneObjectsByTag(string tag) {
	return Instance()->m_TagMap[tag];
}

void DataManager::SetActiveSceneObject(const SceneObject* value) {
	m_ActiveSceneObject = (SceneObject*) value;
}

SceneObject* DataManager::GetActiveSceneObject() {
	return m_ActiveSceneObject;
}

////////////////////////////////////////////////////////////////////////////////
// SceneObjet pool.
////////////////////////////////////////////////////////////////////////////////
unsigned int DataManager::GetPoolSize() {
	return Instance()->m_SceneObjectPool.size();
}

const IdSceneObjectMap& DataManager::GetPool(unsigned int index) {
	return Instance()->m_SceneObjectPool[index];
}

void DataManager::AddSceneObjectToPool(SceneObject* sceneObject) {
	static unsigned int lastPool = 0;
	Instance()->m_SceneObjectPool[lastPool][sceneObject->Id()] = sceneObject;
}

void DataManager::RemoveSceneObjectFromPool(string id) {
	// TODO. This can be improved.
	for(unsigned int i = 0; i<GetPoolSize(); i++) {
		if(Instance()->m_SceneObjectPool[i][id]) {
			Instance()->m_SceneObjectPool[i].erase(id);
			return;
		}
	}
}

void DataManager::ReleaseSceneObjectPool() {
	for(unsigned int i = 0; i<GetPoolSize(); i++) {
		Instance()->m_SceneObjectPool[i].clear();
	}
}

////////////////////////////////////////////////////////////////////////////////
// Mesh.
////////////////////////////////////////////////////////////////////////////////
const shared_ptr<Mesh>& DataManager::GetMesh(const string& filePath, FlagData* flagData) {
	//	If the mesh is not loaded, it loads it and return it.
	if(!Instance()->m_MeshMap[filePath]) {
		auto newMesh = make_shared<Mesh>();

		// Load mesh data in graphic device.
		if(newMesh->LoadMesh(filePath, flagData)) {
			// Add the new mesh to the mesh map.
			Instance()->m_MeshMap[filePath] = newMesh;
		} else {
			Instance()->m_MeshMap[filePath] = nullptr;
		}
	}

	return Instance()->m_MeshMap[filePath];
}

void DataManager::ReleaseMeshMap() {
	Instance()->m_MeshMap.clear();
}

////////////////////////////////////////////////////////////////////////////////
// Lights.
////////////////////////////////////////////////////////////////////////////////
void DataManager::AddLight(string id, Light* light) {
	switch(light->LightType()) {
	case eLightType::DIRECTIONAL_LIGHT:
		Instance()->m_DirectionalLightMap[id] = light;
		break;
	case eLightType::POINT_LIGHT:
		Instance()->m_PointLightMap[id] = light;
		break;
	case eLightType::SPOT_LIGHT:
		Instance()->m_SpotLightMap[id] = light;
		break;
	default:
		break;
	}
}

void DataManager::RemoveLight(string id) {
	Light* light = GetComponentType(SceneObject::FindSceneObjectById(id), Light);

	switch(light->LightType()) {
	case eLightType::DIRECTIONAL_LIGHT:
		Instance()->m_DirectionalLightMap.erase(id);
		break;
	case eLightType::POINT_LIGHT:
		Instance()->m_PointLightMap.erase(id);
		break;
	case eLightType::SPOT_LIGHT:
		Instance()->m_SpotLightMap.erase(id);
		break;
	default:
		break;
	}
}

const DirectionalLightsMap& DataManager::GetDirectionalLightMap() {
	return Instance()->m_DirectionalLightMap;
}

const PointLightsMap& DataManager::GetPointLightMap() {
	return Instance()->m_PointLightMap;
}

const SpotLightsMap& DataManager::GetSpotLightMap() {
	return Instance()->m_SpotLightMap;
}

void DataManager::ReleaseLightsMap() {
	//	Directional map
	Instance()->m_DirectionalLightMap.erase(
		Instance()->m_DirectionalLightMap.begin(),
		Instance()->m_DirectionalLightMap.end());
	//	Point map
	Instance()->m_PointLightMap.erase(
		Instance()->m_PointLightMap.begin(),
		Instance()->m_PointLightMap.end());
	//	Spot map
	Instance()->m_SpotLightMap.erase(
		Instance()->m_SpotLightMap.begin(),
		Instance()->m_SpotLightMap.end());
}

////////////////////////////////////////////////////////////////////////////////
// Shader.
////////////////////////////////////////////////////////////////////////////////
const shared_ptr<Shader>& DataManager::GetShader(const string& nameShader) {
	//	If the shader is not loaded, it loads it and return it.
	if(!Instance()->m_ShaderMap[nameShader]) {
		shared_ptr<Shader> shader;

		if(nameShader==SHADER_GIZMO) {
			shader = make_shared<ShaderGizmo>();
		} else if(nameShader==SHADER_BASE_MESH) {
			shader = make_shared<ShaderBaseMesh>();
		} else if(nameShader==SHADER_SKINNED_MESH) {
			shader = make_shared<ShaderSkinnedMesh>();
		} else if(nameShader==SHADER_SKYBOX) {
			shader = make_shared<ShaderSkybox>();
		} else if(nameShader==SHADER_GEOMETRY) {
			shader = make_shared<ShaderGeometry>();
		} else if(nameShader==SHADER_BLUR) {
			shader = make_shared<ShaderBlur>();
		} else if(nameShader==SHADER_SSAO) {
			shader = make_shared<ShaderSSAO>();
		} else if(nameShader==SHADER_SHADOW_MAP) {
			shader = make_shared<ShaderShadowMap>();
		} else if(nameShader==SHADER_SHADOW_CUBE_MAP) {
			shader = make_shared<ShaderShadowCubeMap>();
		} else if(nameShader==SHADER_POST_PROCESSING) {
			shader = make_shared<ShaderPostProcessing>();
		} else if(nameShader==SHADER_NORMAL_VIEWER) {
			shader = make_shared<ShaderNormalViewer>();
		} else if(nameShader==SHADER_BLOOM) {
			shader = make_shared<ShaderBloom>();
		} else if(nameShader==SHADER_TEXT_RENDERER) {
			shader = make_shared<ShaderTextRenderer>();
		} else if(nameShader==SHADER_LOGO) {
			shader = make_shared<ShaderLogo>();
		} else if(nameShader==SHADER_OCCLUSION_RENDER) {
			shader = make_shared<ShaderOcclusionRender>();
		} else if(nameShader==SHADER_SHADOW_MAP2) {
			shader = make_shared<ShaderShadowMap2>();
		} else if(nameShader==SHADER_SHADOW_CUBE_MAP2) {
			shader = make_shared<ShaderShadowCubeMap2>();
		} else if(nameShader==SHADER_SUBSTATE_MESH_INSTANCED) {
			shader = make_shared<ShaderSubstateMeshInstanced>();
		} else if(nameShader==SHADER_SUBSTATE_MESH_FRONTFACE) {
			shader = make_shared<ShaderSubstateMeshVolumeFrontFace>();
		} else if(nameShader==SHADER_SUBSTATE_MESH_VOLUME) {
			shader = make_shared<ShaderSubstateMeshVolume>();
		} else if(nameShader==SHADER_SUBSTATE_MESH_SURFACE) {
			shader = make_shared<ShaderSubstateMeshSurface>();
		} else if(nameShader==SHADER_INFOBAR_RENDERER) {
			shader = make_shared<ShaderInfoBarRenderer>();
		}

		if(!shader->Initialize()) {
			dg::DebugConsoleMessage("Unable to initialize shader: ");
			dg::DebugConsoleMessage(nameShader.c_str());
			dg::DebugConsoleMessage("\n");
			shader = nullptr;
		}

		Instance()->m_ShaderMap[nameShader] = shader;
	}

	return Instance()->m_ShaderMap[nameShader];
}

void DataManager::ReleaseShaderMap() {
	Instance()->m_ShaderMap.clear();
}

////////////////////////////////////////////////////////////////////////////////
// Texture.
////////////////////////////////////////////////////////////////////////////////
const shared_ptr<Texture>& DataManager::AddTextureToMap(const TextureDesc& textureDesc) {
	shared_ptr<Texture> texture;
	if(!Instance()->m_TextureMap[textureDesc.filePath]) {
		texture = make_shared<Texture>(textureDesc);

		if(texture->Load()) {
			Instance()->m_TextureMap[textureDesc.filePath] = texture;
		}
	}

	return Instance()->m_TextureMap[textureDesc.filePath];
}

const shared_ptr<Texture>& DataManager::GetTexture(const string& fileName) {
	return Instance()->m_TextureMap[fileName];
}

const TextureMap& DataManager::GetTexturesMap() {
	return Instance()->m_TextureMap;
}

const shared_ptr<Texture>& DataManager::TryGetTexture(const TextureDesc& textureDesc) {
	if(Instance()->m_TextureMap[textureDesc.filePath]==nullptr) {
		AddTextureToMap(textureDesc);
	}
	return Instance()->m_TextureMap[textureDesc.filePath];
}

void DataManager::ReleaseTextureMap() {
	Instance()->m_TextureMap.clear();
}

////////////////////////////////////////////////////////////////////////////////
// Material.
////////////////////////////////////////////////////////////////////////////////
const shared_ptr<Material>& DataManager::AddMaterialToMap(const MaterialDesc& materialDesc) {
	Instance()->m_MaterialMap[materialDesc.materialName] = make_shared<Material>(materialDesc);
	return Instance()->m_MaterialMap[materialDesc.materialName];
}

const shared_ptr<Material>& DataManager::GetMaterial(const string& materialName) {
	return Instance()->m_MaterialMap[materialName];
}

const MaterialMap& DataManager::GetMaterialsMap() {
	return Instance()->m_MaterialMap;
}

const shared_ptr<Material>& DataManager::TryGetMaterial(const MaterialDesc& materialDesc) {
	if(Instance()->m_MaterialMap[materialDesc.materialName]==nullptr) {
		AddMaterialToMap(materialDesc);
	}
	return Instance()->m_MaterialMap[materialDesc.materialName];
}

void DataManager::ReleaseMaterialMap() {
	Instance()->m_MaterialMap.clear();
}

void DataManager::Clean() {
	// Clean idMap
	Instance()->m_IdMap.clear();
	// Clean tagMap
	Instance()->m_TagMap.clear();
	// Clean textureMap
	Instance()->ReleaseTextureMap();
	// Clean materialMap
	Instance()->ReleaseMaterialMap();
	// Clean meshMap
	Instance()->ReleaseMeshMap();
	// Clean lightMap
	Instance()->ReleaseLightsMap();
	// Clean instancedMap
	Instance()->ReleaseSceneObjectsMap();
	// Clean object pool.
	Instance()->ReleaseSceneObjectPool();
	// Clean query map.
	Instance()->ReleaseQueryMap();

	Instance()->m_DrawNormalObjects.clear();

	DataManager::m_ActiveSceneObject = nullptr;
}

void DataManager::Release() {
	Instance();

	if(m_Instance) {
		DataManager::ReleaseShaderMap();
		DataManager::Clean();
		Instance()->m_SceneObjectPool.clear();
		m_Instance = nullptr;
	}
}

const unique_ptr<ThreadPool>& DataManager::GetDefaultThreadPool() {
	return Instance()->m_ThreadPool;
}

void DataManager::AddSceneObjectToDrawNormals(SceneObject* sceneObject) {
	Instance()->m_DrawNormalObjects.push_back(sceneObject);
}

void DataManager::RemoveSceneObjectFromDrawNormals(SceneObject* sceneObject) {
	Instance()->m_DrawNormalObjects.remove(sceneObject);
}

std::list<SceneObject*> DataManager::GetDrawNormalObjects() {
	return Instance()->m_DrawNormalObjects;
}

////////////////////////////////////////////////////////////////////////////////
// Instancing.
////////////////////////////////////////////////////////////////////////////////
void DataManager::AddSceneObjectToObjectsMap(const string& filePath, SceneObject* sceneObject) {
	// Generate a query id if the object is new.
	if(!Instance()->m_QueryMap[filePath]) {
		GLuint query = 0;
		glGenQueries(1, &query);
		Instance()->m_QueryMap[filePath] = query;
	}
	Instance()->m_SceneObjectsMap[filePath].push_back(sceneObject);
}

void DataManager::RemoveSceneObjectFromObjectsMap(const string& filePath, SceneObject* sceneObject) {
	Instance()->m_SceneObjectsMap[filePath].remove(sceneObject);
	// Clear query if the list is empty.
	if(Instance()->m_SceneObjectsMap[filePath].empty()) {
		Instance()->m_SceneObjectsMap[filePath].clear();

		// Clear the query id.
		GLuint query = Instance()->m_QueryMap[filePath];
		DELETE_QUERY(query);
		Instance()->m_QueryMap[filePath] = query;
	}
}

const ObjectMap& DataManager::GetSceneObjects() {
	return Instance()->m_SceneObjectsMap;
}

const std::list<SceneObject*>& DataManager::GetSceneObjects(const string& filePath) {
	return Instance()->m_SceneObjectsMap[filePath];
}

void DataManager::ReleaseSceneObjectsMap() {
	ObjectMap* map = &Instance()->m_SceneObjectsMap;
	for(ObjectMap::iterator it = Instance()->m_SceneObjectsMap.begin(); it!=Instance()->m_SceneObjectsMap.end(); it++) {
		// Clear the list.
		it->second.clear();
	}
	Instance()->m_SceneObjectsMap.clear();
}

GLuint DataManager::GetQueryId(const string& filePath) {
	return Instance()->m_QueryMap[filePath];
}

void DataManager::ReleaseQueryMap() {
	for each (auto queryId in Instance()->m_QueryMap) {
		DELETE_QUERY(queryId.second);
	}
	Instance()->m_QueryMap.clear();
}

GLuint DataManager::GetDefaultFramebuffer() {
	return m_DefaultFramebuffer;
}
