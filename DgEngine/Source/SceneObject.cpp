////////////////////////////////////////////////////////////////////////////////
// Filename: SceneObject.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "SceneObject.h"
#include "Behaviour.h"
#include "Camera.h"
#include "Configuration.h"
#include "Component.h"
#include "DataManager.h"
#include "Frustum.h"
#include "Light.h"
#include "MeshRenderer.h"
#include "Script.h"
#include "Transform.h"
#include "UI.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;
using namespace dg;


static unique_ptr<SceneObject> root = nullptr;

#pragma region StaticFunctions
const unique_ptr<SceneObject>& SceneObject::Root() {
	if (!root)	InitRoot();
	return root;
}

SceneObject* SceneObject::FindSceneObjectById(string id) {
	return DataManager::FindSceneObjectById(id);
}

vector<SceneObject*>& SceneObject::FindSceneObjectByTag(string tag) {
	return DataManager::FindSceneObjectsByTag(tag);
}

void SceneObject::UpdateGraphScene() {
	//if(DataManager::GetNumSceneObject()>LIMIT_SCENE_OBJECT) {
	//	const unique_ptr<ThreadPool>& threadPool = DataManager::GetDefaultThreadPool();
	//	for(unsigned int i = 0; i<DataManager::GetPoolSize(); ++i) {
	//		threadPool->SubmitTask([i] {
	//			IdSceneObjectMap map = DataManager::GetPool(i);
	//			for(IdSceneObjectMap::iterator it = map.begin(); it!=map.end(); it++) {
	//				SceneObject::Update(it->second, false);
	//			}
	//		});
	//	}
	//	threadPool->WaitAllTasks();
	//} else {
	//	SceneObject::Update(root.get());
	//}
	SceneObject::Update(root.get());
}

void SceneObject::PostUpdateGraphScene() {
	//if(DataManager::GetNumSceneObject()>LIMIT_SCENE_OBJECT) {
	//	const unique_ptr<ThreadPool>& threadPool = DataManager::GetDefaultThreadPool();
	//	for(unsigned int i = 0; i<DataManager::GetPoolSize(); ++i) {
	//		threadPool->SubmitTask([i] {
	//			for each (auto it in DataManager::GetPool(i)) {
	//				SceneObject::PostUpdate(it.second, false);
	//			}
	//		});
	//	}
	//	threadPool->WaitAllTasks();
	//} else {
	//	SceneObject::PostUpdate(root.get());
	//}
	SceneObject::PostUpdate(root.get());
}

void SceneObject::RenderGraphScene(const shared_ptr<Shader>& shader) {
	// Multiview rendering;
	for each (auto camera in Camera::GetAllCameras()) {
		Camera::SetActiveCamera(camera);	// Set camera as active camera.
		camera->UpdateViewport();			// Update Viewport.
		camera->ComputeMatrices();			// Update view and projection matrices
		// Insert shadow map creation here!

		// Construct frustum.
		Frustum::Instance()->ConstructFrustum();
		Frustum::Instance()->Render();

		// Bounding box rendering.
		if (!shader) {
			SceneObject* activeSceneObject = DataManager::GetActiveSceneObject();
			if (activeSceneObject) {
				MeshRenderer* meshRenderer = GetComponentType(activeSceneObject, MeshRenderer);
				if (meshRenderer)
					meshRenderer->RenderBoundingBox();
			}
		}
		// Normals rendering.
		for each (auto sceneObject in DataManager::GetDrawNormalObjects()) {
			MeshRenderer* meshRenderer = GetComponentType(sceneObject, MeshRenderer);
			if (meshRenderer)
				meshRenderer->RenderNormals();
		}
		// Lights gizmo rendering
#ifdef _DEBUG
		for each (auto &light in DataManager::GetPointLightMap()) {
			light.second->Render();
		}

		// Set directional lights.
		for each (auto &light in DataManager::GetDirectionalLightMap()) {
			light.second->Render();
		}

		// Set spot lights.
		for each (auto &light in DataManager::GetSpotLightMap()) {
			light.second->Render();
		}
#endif
		// Scene rendering.
		for each (auto sceneObject in DataManager::GetSceneObjects()) {
			unsigned int numObjects = sceneObject.second.size();
			if (numObjects > 0)
				Render(sceneObject.second.front(), shader, false, numObjects > 1, DataManager::GetQueryId(sceneObject.first));
		}
	}
}

void SceneObject::OcclusionRenderGraphScene(const shared_ptr<Shader>& shader) {
	// Do occlusion rendering.
	for each (auto sceneObject in DataManager::GetSceneObjects()) {
		unsigned int numObjects = sceneObject.second.size();
		if (numObjects > 0)
			OcclusionRender(sceneObject.second.front(), shader, numObjects > 1, DataManager::GetQueryId(sceneObject.first));
	}
}

void SceneObject::Update(SceneObject* node, bool propagateOnChild) {
	if (node->IsEnabled()) {
		for each (auto &component in node->m_Components)
			if (component.second->IsEnabled())
				component.second->Update();

		if (propagateOnChild)
			for each (auto &child in node->m_Children)
				Update(child);
	}
}

void SceneObject::PostUpdate(SceneObject* node, bool propagateOnChild) {
	if (node->IsEnabled()) {
		for each (auto &component in node->m_Components)
			if (component.second->IsEnabled())
				component.second->PostUpdate();

		if (propagateOnChild)
			for each (auto &child in node->m_Children)
				PostUpdate(child);
	}
}

void SceneObject::Render(SceneObject* node, const shared_ptr<Shader>& shader, bool propagateOnChild, bool instanced, const GLuint& query) {
	if (node->IsEnabled()) {
		// Render only if this scene object belong to the active camera.
		if (node->GetRenderLayer() == Camera::ActiveCamera()->GetRenderLayer())
			for each (auto &component in node->m_Components)
				if (component.second->IsEnabled())
					component.second->Render(shader, instanced, query);

		if (propagateOnChild)
			for each (auto &child in node->m_Children)
				Render(child, shader, propagateOnChild, false, query);
	}
}

void SceneObject::OcclusionRender(SceneObject* node, const shared_ptr<Shader>& shader, const bool& instanced, const GLuint& query) {
	if (node->IsEnabled()) {
		MeshRenderer* meshRenderer = GetComponentType(node, MeshRenderer);
		if (meshRenderer && meshRenderer->IsEnabled())
			meshRenderer->OcclusionRender(shader, query, instanced);
	}
}

void SceneObject::InitRoot() {
	if (!root) {
		root = unique_ptr<SceneObject>(new SceneObject("Root", "Root"));
		Transform* transform = new Transform(root.get());
		transform->GetWorldMatrix();
	}
}

void SceneObject::CleanRoot() {
	for each (auto child in SceneObject::Root()->m_Children) {
		FREE_PTR(child);
	}
	SceneObject::Root()->m_Children.clear();
}

void SceneObject::DeleteRoot() {
	root = nullptr;
}

bool SceneObject::AddScriptToSceneObject(string id, Script* script) {
	SceneObject* sceneObject = SceneObject::FindSceneObjectById(id);
	if (sceneObject) {
		return SceneObject::AddScriptToSceneObject(sceneObject, script);
	}
	FREE_PTR(script);
	return false;
}

bool SceneObject::AddScriptToSceneObject(SceneObject* sceneObject, Script* script) {
	sceneObject->AddScript(script);
	return true;
}

bool SceneObject::DestroySceneObject(string id) {
	SceneObject* sceneObject = SceneObject::FindSceneObjectById(id);
	if (sceneObject) {
		return DestroySceneObject(sceneObject);
	}
	return false;
}

bool SceneObject::DestroySceneObject(SceneObject* sceneObject) {
	sceneObject->Parent()->RemoveChildren(sceneObject);
	delete sceneObject;
	return true;
}
#pragma endregion

SceneObject::SceneObject(string id, string tag, unsigned int renderLayer)
	: m_Id(id), m_Tag(tag), m_RenderLayer(renderLayer) {
	m_Enabled = true;
	m_Parent = nullptr;
	DataManager::AddSceneObject(this);
}

SceneObject::~SceneObject() {
	// Clean father
	m_Parent = nullptr;

	// Clean children
	for each (auto child in m_Children) {
		FREE_PTR(child);
	}
	m_Children.clear();

	// Clean component
	for each (auto component in m_Components) {
		FREE_PTR(component.second);
	}
	m_Components.clear();

	// Clean from id and tag map
	DataManager::RemoveSceneObject(m_Id);
}

void SceneObject::DrawUI(TwBar* bar) {
	char parameter[128];
	memset(parameter, 0, sizeof(parameter));

	// Bar movable?
	SNPRINTF(parameter, sizeof(parameter), "%s movable=false", Id().c_str());
	TwDefine(parameter);

	// Bar size.
	int sizeX = 240;
	int sizeY = Configuration::Instance()->GetScreenHeigth() - 20;
	SNPRINTF(parameter, sizeof(parameter), "%s  size='%d %d' ", Id().c_str(), sizeX, sizeY);
	TwDefine(parameter);

	// Bar position.
	int posX = Configuration::Instance()->GetScreenWidth() - sizeX - 10;
	SNPRINTF(parameter, sizeof(parameter), "%s position='%d 10' ", Id().c_str(), posX);
	TwDefine(parameter);

	TwAddVarCB(bar, "Enabled", TW_TYPE_BOOL8, EnabledCallback, IsEnabledCallback, this, "");
	string tagStr = "Tag: " + Tag();
	TwAddButton(bar, tagStr.c_str(), NULL, NULL, "");

	TwAddVarCB(bar, "SceneObjectRenderLayer", TW_TYPE_UINT32, SetRenderLayerCallback, GetRenderLayerCallback, this, " label='RenderLayer' ");
	SNPRINTF(parameter, sizeof(parameter), "%s/SceneObjectRenderLayer min=%d ", Id().c_str(), 0);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/SceneObjectRenderLayer step=%d ", Id().c_str(), 1);
	TwDefine(parameter);

	TwAddSeparator(bar, "", NULL);
	TwAddSeparator(bar, "", NULL);

	// For all components draw the UI.
	for (unsigned int i = 0; i < eComponentType::NUM_COMPONENTs; i++) {
		if (GetComponent(eComponentType(i))) {
			GetComponent(eComponentType(i))->DrawUI(bar);
		}
	}
}

string SceneObject::Id() const {
	return m_Id;
}

string SceneObject::Tag() const {
	return m_Tag;
}

void SceneObject::AddChild(SceneObject* child) {
	child->m_Parent = this;
	m_Children.push_back(child);
}

SceneObject* SceneObject::Parent() const {
	return m_Parent;
}

void SceneObject::RemoveChildrenById(string id) {
	RemoveChildren(DataManager::FindSceneObjectById(id));
}

void SceneObject::RemoveChildren(SceneObject* child) {
	for (vector<SceneObject*>::iterator it = m_Children.begin(); it != m_Children.end(); it++) {
		if ((*it) == child) {
			m_Children.erase(it);
			DataManager::RemoveSceneObject(child->m_Id);
			if (*it) {
				delete *it;
			}
			return;
		}
	}
	return;
}

const vector<SceneObject*>& SceneObject::Children() const {
	return m_Children;
}

void SceneObject::Enable(bool value) {
	m_Enabled = value;
}

void SceneObject::Disable() {
	m_Enabled = false;
}

bool SceneObject::IsEnabled() {
	return m_Enabled;
}

void SceneObject::SetComponent(Component* component) {
	FREE_PTR(m_Components[component->ComponentType()]);
	if (!m_Components[component->ComponentType()]) {
		m_Components[component->ComponentType()] = component;
	}
}

Component* SceneObject::GetComponent(const eComponentType& type) const {
	ComponentMap::const_iterator item = m_Components.find(type);
	if (item != m_Components.end()) {
		return item->second;
	}
	return nullptr;
}

void SceneObject::AddScript(Script* script) {
	Behaviour* behaviour = nullptr;
	if (GetComponent(eComponentType::BEHAVIOUR)) {
		behaviour = (Behaviour*)GetComponent(eComponentType::BEHAVIOUR);
	}
	else {
		behaviour = new Behaviour(this);
	}
	behaviour->AddScript(script);
}

void SceneObject::RemoveScript(Script* script) {
	Behaviour* behaviour = (Behaviour*)GetComponent(eComponentType::BEHAVIOUR);
	if (behaviour) {
		behaviour->RemoveScript(script);
	}
}

void SceneObject::SetRenderLayer(const unsigned int& value) {
	m_RenderLayer = value;
}

const unsigned int& SceneObject::GetRenderLayer() const {
	return m_RenderLayer;
}

#pragma region OperatorOverloading
bool SceneObject::operator==(const SceneObject &other) const {
	return m_Id == other.m_Id;
}

bool SceneObject::operator!=(const SceneObject &other) const {
	return !(*this == other);
}
#pragma endregion
