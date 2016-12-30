////////////////////////////////////////////////////////////////////////////////
// Filename: MeshRenderer.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////
#include <list>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "MeshRenderer.h"
#include "Camera.h"
#include "Configuration.h"
#include "DataManager.h"
#include "Environment.h"
#include "Frustum.h"
#include "Glossary.h"
#include "Transform.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


#pragma region Callbacks
void TW_CALL SetDrawNormalsCallback(const void *value, void *clientData) {
	static_cast<MeshRenderer *>(clientData)->DrawNormals(*static_cast<const bool *>(value));
}
void TW_CALL GetDrawNormalsCallback(void *value, void *clientData) {
	*static_cast<bool *>(value) = static_cast<MeshRenderer *>(clientData)->IsDrawNormals();
}
void TW_CALL SetShadelessCallback(const void *value, void *clientData) {
	static_cast<MeshRenderer *>(clientData)->SetShadeless(*static_cast<const bool *>(value));
}
void TW_CALL GetShadelessCallback(void *value, void *clientData) {
	*static_cast<bool *>(value) = static_cast<MeshRenderer *>(clientData)->IsShadeless();
}
#pragma endregion

MeshRenderer::MeshRenderer(SceneObject* sceneObject)
	: Component(eComponentType::MESH_RENDERER, sceneObject) {
	m_Mesh = nullptr;
	m_DrawNormals = false;
	m_NormalMagnitude = 0.5f;
	m_Shadeless = false;
}

MeshRenderer::~MeshRenderer() {
	m_Mesh = nullptr;
	m_Materials.clear();
}

void MeshRenderer::Update() {

}

void MeshRenderer::PostUpdate() {

}

void MeshRenderer::Render(const shared_ptr<Shader>& shader, bool instanced, const GLuint& query) {
	if (Environment::Instance()->IsFrustumCullingActive()) {
		// Check if inside or outside the frustum.
		vec3 min = m_Mesh->GetMinExtension();
		vec3 max = m_Mesh->GetMaxExtension();
		Transform* transform = GetComponentType(GetSceneObject(), Transform);
		if(!Frustum::Instance()->IsBoxInFrustum(transform->GetPosition() + min, transform->GetPosition() + max))
			return;
	}

	// Set parameters to the shader.
	SetShaderParameters(shader, instanced);

	if (instanced) {
		Render_Instanced(shader, query);
	}
	else {
		Render_Standard(shader, query);
	}
}

void MeshRenderer::OcclusionRender(const shared_ptr<Shader>& shader, const GLuint& query, const bool& instanced) {
	shader->SetActive();
	// Set if use instanced rendering to the shader.
	shader->SetInt("Instancing", instanced);
	shader->SetParameters(GetSceneObject());

	vector<mat4> mvpMatrices;
	if (instanced) {
		list<SceneObject*> instancedMesh = DataManager::GetSceneObjects(m_FilePath);
		mvpMatrices.reserve(instancedMesh.size());
		for each (auto sceneObject in instancedMesh) {
			Transform* transform = GetComponentType(sceneObject, Transform);
			mvpMatrices.push_back(transform->GetWorldViewProjectionMatrix());
		}
	}
	else {
		mvpMatrices.push_back(mat4(1.0f));
	}

	m_Mesh->OcclusionQuery(query, mvpMatrices);
}

void MeshRenderer::DrawUI(TwBar* bar) {
	char parameter[128];
	memset(parameter, 0, sizeof(parameter));

	// Mesh component values.
	TwAddButton(bar, "MeshRenderer", NULL, NULL, "");
	TwAddVarCB(bar, "Mesh Enabled", TW_TYPE_BOOL8, MeshRenderer::EnabledCallback, MeshRenderer::IsEnabledCallback, this, "");
	TwAddVarRO(bar, "Path: ", TW_TYPE_STDSTRING, &m_FilePath, "");
	TwAddVarCB(bar, "Shadeless", TW_TYPE_BOOL8, SetShadelessCallback, GetShadelessCallback, this, "");
	TwAddButton(bar, "Normals", NULL, NULL, "");
	TwAddVarCB(bar, "Display normals", TW_TYPE_BOOL8, SetDrawNormalsCallback, GetDrawNormalsCallback, this, "");
	TwAddVarRW(bar, "NormalsMagnitude", TW_TYPE_FLOAT, &m_NormalMagnitude, " label='Magnitude' ");
	SNPRINTF(parameter, sizeof(parameter), "%s/NormalsMagnitude min=%g", GetSceneObject()->Id().c_str(), 0.0f);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/NormalsMagnitude step=%g ", GetSceneObject()->Id().c_str(), 0.1f);
	TwDefine(parameter);
	Material::DrawUIMaterials(bar, m_Materials);
	TwAddSeparator(bar, "", NULL);
	TwAddSeparator(bar, "", NULL);
}

void MeshRenderer::Render_Standard(const shared_ptr<Shader>& shader, const GLuint& query) {
	if (shader) {
		m_Mesh->Render(query, nullptr, GL_TRIANGLES);
	}
	else {
		m_Mesh->Render(query, &m_Materials);
	}
}

void MeshRenderer::Render_Instanced(const shared_ptr<Shader>& shader, const GLuint& query) {
	list<SceneObject*> instancedMesh = DataManager::GetSceneObjects(m_FilePath);

	unsigned int numMesh = 0;
	vector<mat4> MVPMatrices;
	vector<mat4> ModelMatrices;

	MVPMatrices.reserve(instancedMesh.size());
	ModelMatrices.reserve(instancedMesh.size());

	for each (auto sceneObject in instancedMesh) {
		/*
		*	Draw an object only if:
		*	- it is active,
		*	- its MeshRenderer is active,
		*	- it is on the same render layer with the active camera.
		*/
		if (sceneObject->IsEnabled() &&
			Camera::ActiveCamera()->GetRenderLayer() == sceneObject->GetRenderLayer()) {
			Transform* transform = GetComponentType(sceneObject, Transform);
			MeshRenderer* meshRenderer = GetComponentType(sceneObject, MeshRenderer);
			if (meshRenderer->IsEnabled()) {
				MVPMatrices.push_back(transform->GetWorldViewProjectionMatrix());
				ModelMatrices.push_back(transform->GetWorldMatrix());
				numMesh++;
			}
		}
	}

	m_Mesh->SetMVPMatrices(MVPMatrices);
	m_Mesh->UpdateMVPMatrices();
	m_Mesh->SetMMatrices(ModelMatrices);
	m_Mesh->UpdateMMatrices();

	if (shader) {
		m_Mesh->RenderInstanced(query);
	}
	else {
		m_Mesh->RenderInstanced(query, &m_Materials);
	}

	MVPMatrices.clear();
	ModelMatrices.clear();
}

void MeshRenderer::RenderNormals() {
	if (m_Mesh == nullptr)
		return;
	Shader* shader = DataManager::GetShader(SHADER_NORMAL_VIEWER).get();
	shader->SetParameters(GetSceneObject());
	shader->SetFloat("Magnitude", m_NormalMagnitude);
	m_Mesh->Render(0);
}

void MeshRenderer::SetShaderParameters(const shared_ptr<Shader>& shader, bool instanced) {
	static shared_ptr<Shader> lastShader = nullptr;

	// If there is an external shader use it,
	// otherwise use the material shader.
	if (shader) {
		lastShader = shader;
	}
	else {
		// If all materials share the same shader,
		// I set the SceneObject values only one time.
		for each (auto material in m_Materials) {
			if (material->GetShader() != lastShader) {
				lastShader = material->GetShader();
			}
			lastShader->SetActive();
		}
	}

	// Set if use instanced rendering to the shader.
	lastShader->SetInt("Instancing", instanced);

	// Set if shadeless or not.
	lastShader->SetInt("Shadeless", m_Shadeless);

	lastShader->SetParameters(GetSceneObject());

	dg::CheckGLError();
}

void MeshRenderer::LoadMesh(const string& filePath, FlagData* flagData, const vector<shared_ptr<Material>>* materials) {
	m_FilePath = filePath;
	m_Mesh = DataManager::GetMesh(m_FilePath, flagData);

	// Add to map for instanced rendering.
	DataManager::AddSceneObjectToObjectsMap(m_FilePath, GetSceneObject());

	if (materials) {
		// Add user defined materials.
		SetMaterials(*materials);
	}
	else {
		// Add loaded mesh materials.
		SetMaterials(m_Mesh->GetDefaultMaterials());
	}
}

const shared_ptr<Mesh>& MeshRenderer::GetMesh() {
	return m_Mesh;
}

void MeshRenderer::SetMaterials(const vector<shared_ptr<Material>>& materials) {
	// Add user defined materials.
	for each (auto material in materials) {
		m_Materials.push_back(material);
	}
}

void MeshRenderer::RenderBoundingBox() {
	Transform* transform = GetComponentType(GetSceneObject(), Transform);
	m_Mesh->RenderBoundingBox(transform);
}

void MeshRenderer::DrawNormals(const bool& value) {
	m_DrawNormals = value;
	if (m_DrawNormals) {
		DataManager::AddSceneObjectToDrawNormals(GetSceneObject());
	}
	else {
		DataManager::RemoveSceneObjectFromDrawNormals(GetSceneObject());
	}
}

const bool& MeshRenderer::IsDrawNormals() const {
	return m_DrawNormals;
}

void MeshRenderer::SetShadeless(const bool& value) {
	m_Shadeless = value;
}

const bool& MeshRenderer::IsShadeless() const {
	return m_Shadeless;
}

const vector<shared_ptr<Material>>& MeshRenderer::GetMaterials() const {
	return m_Materials;
}
