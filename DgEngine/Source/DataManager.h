////////////////////////////////////////////////////////////////////////////////
// Filename: DataManager.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Common.h"
#include "ThreadPool.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class Camera;
	class SceneObject;
	class Light;
	class Material;
	class Mesh;
	class Shader;
	class Texture;
	class OculusVR;
	class HdrFBO;
	
	// SceneObject.
	typedef map<string, SceneObject*, less<string>>			IdSceneObjectMap;
	typedef map<string, vector<SceneObject*>, less<string>>	TagSceneObjectsMap;
	// Mesh.
	typedef map<string, shared_ptr<Mesh>, less<string>>		MeshMap;
	// Shader.
	typedef map<string, shared_ptr<Shader>, less<string>>	ShaderMap;
	// Lights.
	typedef map<string, Light*, less<string>>				DirectionalLightsMap;
	typedef map<string, Light*, less<string>>				PointLightsMap;
	typedef map<string, Light*, less<string>>				SpotLightsMap;
	// Textures.
	typedef map<string, shared_ptr<Texture>, less<string>>	TextureMap;
	// Materials.
	typedef map<string, shared_ptr<Material>, less<string>>	MaterialMap;
	// SceneObjectPool.
	typedef vector<IdSceneObjectMap>						SceneObjectPool;
	// Instancing
	typedef map<string, list<SceneObject*>, less<string>>	ObjectMap;
	typedef map<string, GLuint, less<string>>				QueryMap;
	
	class DataManager {
		friend class Camera;
		friend class SceneObject;
		friend class OculusVR;
		friend class HdrFBO;

	public:
		static const unique_ptr<DataManager>& Instance();
		// SceneObjet.
		static unsigned int GetNumSceneObject();
		static void AddSceneObject(SceneObject*);
		static void RemoveSceneObject(string);
		static SceneObject* FindSceneObjectById(string);
		static vector<SceneObject*>& FindSceneObjectsByTag(string);
		static void SetActiveSceneObject(const SceneObject*);
		static SceneObject* GetActiveSceneObject();
		// SceneObject pool.
		static unsigned int GetPoolSize();
		static const IdSceneObjectMap& GetPool(unsigned int);
		static void AddSceneObjectToPool(SceneObject*);
		static void RemoveSceneObjectFromPool(string);
		static void ReleaseSceneObjectPool();
		// Mesh.
		static const shared_ptr<Mesh>& GetMesh(const string&, FlagData* = nullptr);
		static void ReleaseMeshMap();
		// Lights.
		static void AddLight(string, Light*);
		static void RemoveLight(string);
		static const DirectionalLightsMap& GetDirectionalLightMap();
		static const PointLightsMap& GetPointLightMap();
		static const SpotLightsMap& GetSpotLightMap();
		static void ReleaseLightsMap();
		// Shader.
		static const shared_ptr<Shader>& GetShader(const string&);
		static void ReleaseShaderMap();
		// Textures.
		static const shared_ptr<Texture>& AddTextureToMap(const TextureDesc&);
		static const shared_ptr<Texture>& GetTexture(const string&);
		static const TextureMap& GetTexturesMap();
		static const shared_ptr<Texture>& TryGetTexture(const TextureDesc&);
		static void ReleaseTextureMap();
		// Materials.
		static const shared_ptr<Material>& AddMaterialToMap(const MaterialDesc&);
		static const shared_ptr<Material>& GetMaterial(const string&);
		static const MaterialMap& GetMaterialsMap();
		static const shared_ptr<Material>& TryGetMaterial(const MaterialDesc&);
		static void ReleaseMaterialMap();
		// Draw normals.
		static void AddSceneObjectToDrawNormals(SceneObject*);
		static void RemoveSceneObjectFromDrawNormals(SceneObject*);
		static std::list<SceneObject*> GetDrawNormalObjects();
		// Instancing.
		static void AddSceneObjectToObjectsMap(const string&, SceneObject*);
		static void RemoveSceneObjectFromObjectsMap(const string&, SceneObject*);
		static const ObjectMap& GetSceneObjects();
		static const std::list<SceneObject*>& GetSceneObjects(const string&);
		static void ReleaseSceneObjectsMap();
		static const unique_ptr<ThreadPool>& GetDefaultThreadPool();
		static GLuint GetQueryId(const string&);
		static void ReleaseQueryMap();
		static GLuint GetDefaultFramebuffer();

		static void Clean();
		static void Release();

	public:
		virtual ~DataManager();

	private:
		DataManager();

	private:
		static GLuint					m_DefaultFramebuffer;
		static unique_ptr<DataManager>	m_Instance;
		static SceneObject*				m_ActiveSceneObject;
		IdSceneObjectMap				m_IdMap;
		TagSceneObjectsMap				m_TagMap;
		SceneObjectPool					m_SceneObjectPool;
		Camera*							m_MainCamera;
		vector<Camera*>					m_Cameras;
		MeshMap							m_MeshMap;
		ShaderMap						m_ShaderMap;
		DirectionalLightsMap			m_DirectionalLightMap;
		PointLightsMap					m_PointLightMap;
		SpotLightsMap					m_SpotLightMap;
		TextureMap						m_TextureMap;
		MaterialMap						m_MaterialMap;
		unique_ptr<ThreadPool>			m_ThreadPool;
		// Draw normals.
		list<SceneObject*>				m_DrawNormalObjects;
		// Instancing.
		ObjectMap						m_SceneObjectsMap;
		QueryMap						m_QueryMap;
	};
}
