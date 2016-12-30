////////////////////////////////////////////////////////////////////////////////
// Filename: Gizmo.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Gizmo.h"
#include "DataManager.h"
#include "Environment.h"
#include "Glossary.h"
#include "Transform.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


#define DEFAULT_GRID_SIZE 10
#ifdef _DEBUG
#define DRAW_GIZMO
#endif

unique_ptr<Gizmo> Gizmo::m_Instance = nullptr;

Gizmo::Gizmo() {
	m_GridSize = -1;

	m_Shader = shared_ptr<Shader>(DataManager::GetShader(SHADER_GIZMO));

	m_MeshesPath.resize(eGizmo::NUM_GIZMOs);

	m_MeshesPath[eGizmo::AXIS_GIZMO] = AXIS_MESH_PATH;
	m_MeshesPath[eGizmo::DIRECTIONAL_LIGHT_GIZMO] = DIRECTIONAL_LIGHT_MESH_PATH;
	m_MeshesPath[eGizmo::POINT_LIGHT_GIZMO] = POINT_LIGHT_MESH_PATH;
	m_MeshesPath[eGizmo::SPOT_LIGHT_GIZMO] = SPOT_LIGHT_MESH_PATH;
	m_MeshesPath[eGizmo::CUBE_GIZMO] = CUBE_MESH_PATH;

	m_Meshes.resize(eGizmo::NUM_GIZMOs);
	for(unsigned int i = 0; i<eGizmo::NUM_GIZMOs; i++) {
		m_Meshes[i] = nullptr;
	}
}

Gizmo::~Gizmo() {
	m_Shader = nullptr;
	m_Grid_1 = nullptr;
	m_Grid_2 = nullptr;
	m_Meshes.clear();
	m_MeshesPath.clear();
}

const unique_ptr<Gizmo>& Gizmo::Instance() {
	if(!m_Instance) {
		m_Instance = unique_ptr<Gizmo>(new Gizmo());
	}
	return m_Instance;
}

void Gizmo::Release() {
	m_Instance = nullptr;
}

void Gizmo::DrawGrid(const vec3& position, const quat& rotation, const vec3& scaling) {
#ifdef DRAW_GIZMO
	const unique_ptr<Gizmo>& instancePtr = Instance();

	if(!instancePtr->m_Grid_1) {
		instancePtr->InitializeGrid();
	}

	instancePtr->m_Shader->SetActive();
	instancePtr->m_Shader->SetInt("AlbedoMap", COLOR_TEXTURE_UNIT_INDEX);
	instancePtr->m_Shader->SetInt("AlbedoMap_bool", false);
	instancePtr->m_Shader->SetInt("ActiveOIT", Environment::Instance()->IsOITActive());

	Transform transform(NULL);
	transform.SetPosition(position);
	transform.SetRotation(rotation);
	transform.SetScaling(scaling);

	instancePtr->m_Shader->SetFloatMatrix4("MVPMatrix", transform.GetWorldViewProjectionMatrix());

	instancePtr->m_Shader->SetFloatVector4("Color", color_white);
	instancePtr->m_Grid_2->Render(NULL_QUERY, NULL, GL_LINES);
	instancePtr->m_Shader->SetFloatVector4("Color", color_white*0.5f);
	instancePtr->m_Grid_1->Render(NULL_QUERY, NULL, GL_LINES);
#endif
}

void Gizmo::DrawGizmo(eGizmo gizmo, const vec3& position, const quat& rotation, const vec3& scaling) {
#ifdef DRAW_GIZMO
	const unique_ptr<Gizmo>& instancePtr = Instance();

	if(!instancePtr->m_Meshes[gizmo]) {
		instancePtr->m_Meshes[gizmo] = DataManager::GetMesh(Instance()->m_MeshesPath[gizmo]);

		// Set shader to materials.
		vector<shared_ptr<Material>> materials = instancePtr->m_Meshes[gizmo]->GetDefaultMaterials();
		for each (auto var in materials) {
			var->SetShader(instancePtr->m_Shader);
			var->SetWireframe(false);
		}
	}
	if(instancePtr->m_Meshes[gizmo]) {
		instancePtr->m_Shader->SetActive();
		instancePtr->m_Shader->SetInt("AlbedoMap", COLOR_TEXTURE_UNIT_INDEX);
		instancePtr->m_Shader->SetInt("AlbedoMap_bool", 1);
		instancePtr->m_Shader->SetInt("ActiveOIT", Environment::Instance()->IsOITActive());

		Transform transform(NULL);
		transform.SetPosition(position);
		transform.SetRotation(rotation);
		transform.SetScaling(scaling);

		instancePtr->m_Shader->SetFloatMatrix4("MVPMatrix", transform.GetWorldViewProjectionMatrix());
		instancePtr->m_Meshes[gizmo]->Render(NULL_QUERY, &instancePtr->m_Meshes[gizmo]->GetDefaultMaterials());
	}
#endif
}

void Gizmo::DrawGizmo(eGizmo gizmo, Transform* transform, const vec3& scaling) {
#ifdef DRAW_GIZMO
	const unique_ptr<Gizmo>& instancePtr = Instance();

	if(!instancePtr->m_Meshes[gizmo]) {
		instancePtr->m_Meshes[gizmo] = DataManager::GetMesh(Instance()->m_MeshesPath[gizmo]);

		// Set shader to materials.
		vector<shared_ptr<Material>> materials = instancePtr->m_Meshes[gizmo]->GetDefaultMaterials();
		for each (auto var in materials) {
			var->SetShader(instancePtr->m_Shader);
			var->SetWireframe(false);
		}
	}
	if(instancePtr->m_Meshes[gizmo]) {
		instancePtr->m_Shader->SetActive();
		instancePtr->m_Shader->SetInt("AlbedoMap", COLOR_TEXTURE_UNIT_INDEX);
		instancePtr->m_Shader->SetInt("AlbedoMap_bool", 1);
		instancePtr->m_Shader->SetInt("ActiveOIT", Environment::Instance()->IsOITActive());

		mat4 mvpMatrix = transform->GetViewProjectionMatrix() *
			//transform->GetParentTransformMatrix() *
			glm::translate(mat4(1.0f), transform->GetPosition()) *
			glm::toMat4(transform->GetRotation()) *
			glm::scale(mat4(1.0f), scaling);

		instancePtr->m_Shader->SetFloatMatrix4("MVPMatrix", mvpMatrix);

		instancePtr->m_Meshes[gizmo]->Render(NULL_QUERY, &instancePtr->m_Meshes[gizmo]->GetDefaultMaterials());
	}
#endif
}

void Gizmo::SetGridSize(int size) {
#ifdef DRAW_GIZMO
	if(Instance()->m_GridSize!=size) {
		if(size%2!=0&&size%5!=0)
			return;
		Instance()->m_GridSize = size;
		Instance()->InitializeGrid();
	}
#endif
}

void Gizmo::InitializeGrid() {
	if(m_GridSize<0)
		m_GridSize = DEFAULT_GRID_SIZE;

	// Grid 1
	auto gridMesh = make_shared<Mesh>();

	vector<vec3> positions;
	vector<vec3> normals;
	vector<vec2> texCoords;
	vector<vec3> tangents;
	vector<unsigned int> indices;

	int max = m_GridSize/2;
	int min = -max;
	int cellSpacing = 1;

	unsigned int numVertices = (m_GridSize+1)*4/cellSpacing;
	numVertices -= (m_GridSize+1)*4/5;
	unsigned int numIndices = numVertices;
	positions.reserve(numVertices);
	normals.reserve(numVertices);
	texCoords.reserve(numVertices);
	tangents.reserve(numVertices);
	indices.reserve(numVertices);

	int indexIndices = 0;
	// Orizontal Lines
	for(int i = min; i<=max; i += cellSpacing) {
		if((i-min)%(5)!=0) {
			positions.push_back(vec3((float) min, 0.0f, (float) i));
			texCoords.push_back(vec2(0.0f, 0.0f));
			indices.push_back(indexIndices++);
			positions.push_back(vec3((float) max, 0.0f, (float) i));
			texCoords.push_back(vec2(0.0f, 0.0f));
			indices.push_back(indexIndices++);
		}
	}
	// Vertical Lines
	for(int i = min; i<=max; i += cellSpacing) {
		if((i-min)%(5)!=0) {
			positions.push_back(vec3((float) i, 0.0f, (float) max));
			texCoords.push_back(vec2(0.0f, 0.0f));
			indices.push_back(indexIndices++);
			positions.push_back(vec3((float) i, 0.0f, (float) min));
			texCoords.push_back(vec2(0.0f, 0.0f));
			indices.push_back(indexIndices++);
		}
	}

	if(gridMesh->CreateMesh(positions, normals, texCoords, tangents, indices)) {
		m_Grid_1 = gridMesh;
	} else {
		gridMesh = nullptr;
	}

	positions.clear();
	normals.clear();
	texCoords.clear();
	tangents.clear();
	indices.clear();

	// Grid 2
	gridMesh.reset(new Mesh());

	max = m_GridSize/2;
	min = -max;
	cellSpacing = 5;

	numVertices = (m_GridSize+1)*4/cellSpacing;
	numIndices = numVertices;
	positions.reserve(numVertices);
	normals.reserve(numVertices);
	texCoords.reserve(numVertices);
	tangents.reserve(numVertices);
	indices.reserve(numVertices);

	indexIndices = 0;
	// Orizontal Lines
	for(int i = min; i<=max; i += cellSpacing) {
		positions.push_back(vec3((float) min, 0.0f, (float) i));
		texCoords.push_back(vec2(0.0f, 0.0f));
		indices.push_back(indexIndices++);
		positions.push_back(vec3((float) max, 0.0f, (float) i));
		texCoords.push_back(vec2(0.0f, 0.0f));
		indices.push_back(indexIndices++);
	}
	// Vertical Lines
	for(int i = min; i<=max; i += cellSpacing) {
		positions.push_back(vec3((float) i, 0.0f, (float) max));
		texCoords.push_back(vec2(0.0f, 0.0f));
		indices.push_back(indexIndices++);
		positions.push_back(vec3((float) i, 0.0f, (float) min));
		texCoords.push_back(vec2(0.0f, 0.0f));
		indices.push_back(indexIndices++);
	}

	if(gridMesh->CreateMesh(positions, normals, texCoords, tangents, indices)) {
		m_Grid_2 = gridMesh;
	} else {
		gridMesh = nullptr;
	}

	positions.clear();
	normals.clear();
	texCoords.clear();
	tangents.clear();
	indices.clear();
}
