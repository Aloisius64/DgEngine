////////////////////////////////////////////////////////////////////////////////
// Filename: SceneObject.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <AntTweakBar.h>
#include <memory>
#include <string>
#include <vector>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Common.h"
#include "Mathematics.h"
#include "Shader.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


#define GetComponentType(sceneObject, component)\
	(component*) sceneObject->GetComponent(component::ComponentType())

namespace dg {
	class Component;
	class Script;
	class Shader;

	typedef map<eComponentType, Component*, less<eComponentType>>	ComponentMap;

	class SceneObject {
	public:
		static const unique_ptr<SceneObject>& Root();
		static SceneObject* FindSceneObjectById(string);
		static vector<SceneObject*>& FindSceneObjectByTag(string);
		static void UpdateGraphScene();
		static void PostUpdateGraphScene();
		static void RenderGraphScene(const shared_ptr<Shader>& = nullptr);
		static void OcclusionRenderGraphScene(const shared_ptr<Shader>&);
		static void Update(SceneObject*, bool = true);
		static void PostUpdate(SceneObject*, bool = true);
		static void Render(SceneObject*, const shared_ptr<Shader>& = nullptr, bool = true, bool = false, const GLuint& = NULL_QUERY);
		static void OcclusionRender(SceneObject*, const shared_ptr<Shader>&, const bool& = false, const GLuint& = NULL_QUERY);
		static void InitRoot();
		static void CleanRoot();
		static void DeleteRoot();
		static bool AddScriptToSceneObject(string, Script*);
		static bool AddScriptToSceneObject(SceneObject*, Script*);
		static bool DestroySceneObject(string);
		static bool DestroySceneObject(SceneObject*);

	public:
		SceneObject(string = "Unknow_ID", string = "Unknow_TAG", unsigned int = 0);
		virtual ~SceneObject();

		virtual void DrawUI(TwBar*);

		string Id() const;
		string Tag() const;
		void Enable(bool = true);
		void Disable();
		bool IsEnabled();

		void AddChild(SceneObject*);
		void RemoveChildren(SceneObject*);
		void RemoveChildrenById(string);
		const vector<SceneObject*>& Children() const;
		SceneObject* Parent() const;

		Component* GetComponent(const eComponentType&) const;
		void SetComponent(Component*);

		void AddScript(Script*);
		void RemoveScript(Script*);

		void SetRenderLayer(const unsigned int&);
		const unsigned int& GetRenderLayer() const;

		bool operator==(const SceneObject&) const;
		bool operator!=(const SceneObject&) const;

	protected:
#pragma region AntTweakBar
		static void TW_CALL EnabledCallback(const void *value, void *clientData) {
			static_cast<SceneObject *>(clientData)->Enable(*static_cast<const bool *>(value));
		}
		static void TW_CALL IsEnabledCallback(void *value, void *clientData) {
			*static_cast<bool *>(value) = static_cast<SceneObject*>(clientData)->IsEnabled();
		}
		static void TW_CALL SetRenderLayerCallback(const void *value, void *clientData) {
			static_cast<SceneObject *>(clientData)->SetRenderLayer(*static_cast<const unsigned int *>(value));
		}
		static void TW_CALL GetRenderLayerCallback(void *value, void *clientData) {
			*static_cast<unsigned int *>(value) = static_cast<SceneObject*>(clientData)->GetRenderLayer();
		}
#pragma endregion

	protected:
		string					m_Id;
		string					m_Tag;
		bool					m_Enabled;
		SceneObject*			m_Parent;
		vector<SceneObject*>	m_Children;
		ComponentMap			m_Components;
		unsigned int			m_RenderLayer;
	};
}
