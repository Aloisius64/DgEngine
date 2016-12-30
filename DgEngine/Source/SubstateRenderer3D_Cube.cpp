////////////////////////////////////////////////////////////////////////////////
// Filename: SubstateRenderer3D_Cube.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////
#include <AntTweakBar.h>
#include <memory>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "SubstateRenderer.h"
#include "Behaviour.h"
#include "Mesh.h"
#include "Configuration.h"
#include "DataManager.h"
#include "Environment.h"
#include "Glossary.h"
#include "SubstateMesh.h"
#include "Transform.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


class SubstateRenderer3D_Cube : public SubstateRenderer {
public:
	SubstateRenderer3D_Cube(SceneObject* sceneObject, const SubstateRendererDesc& desc)
		:SubstateRenderer(sceneObject, desc) {
		SET_SUBSTATE_3D_DATA;
		m_CubeMesh = nullptr;
	}

	virtual ~SubstateRenderer3D_Cube() {
		SET_NULLPTR_TO_OPENCAL_DATA;
		m_CubeMesh = nullptr;
	}

	virtual void Update() {
		DRAW_INFOBAR;

		if(m_DataType==eDataType::STATIC_DATA)
			return;

		COMPUTE_EXTREMES_3D;

		if((m_CellularAutomata->GetStep()-m_LastUpdateStep)>=m_CellularAutomata->GetRenderStep()) {
			m_LastUpdateStep = m_CellularAutomata->GetStep();
			int dimension = m_CellularAutomata->GetRows()*m_CellularAutomata->GetColumns()*m_CellularAutomata->GetSlices();
			if(m_CodeSubstate==CODE_SUBSTATE_BYTE) {
				((SubstateMesh*) m_CubeMesh.get())->UpdateByteData(m_ByteSubstate->current, dimension);
			} else if(m_CodeSubstate==CODE_SUBSTATE_INT) {
				((SubstateMesh*) m_CubeMesh.get())->UpdateIntData(m_IntSubstate->current, dimension);
			} else if(m_CodeSubstate==CODE_SUBSTATE_REAL) {
				((SubstateMesh*) m_CubeMesh.get())->UpdateRealData(m_RealSubstate->current, dimension);
			}
		}
	}

	virtual void Render(const shared_ptr<Shader>& shader, bool instanced, const GLuint& query) {
		if(shader!=nullptr)
			return;

		// Set parameters to the shader.
		SetShaderParameters(shader, true);

		m_ShaderSubstateMesh->SetActive();
		SET_SHADER_SUBTATE_PARAMETERS(m_ShaderSubstateMesh);

		m_CubeMesh->RenderInstanced(NULL_QUERY, shader ? NULL : &m_Materials);
	}

protected:
	virtual void Initialize() {
		SubstateRenderer::Initialize();

		// Computing the data for OpenGL buffers.
		vector<vec3> positions;
		vec3 translation;
		translation.x = m_CellularAutomata->GetColumns()/2.0f;
		translation.y = m_CellularAutomata->GetSlices()/2.0f;
		translation.z = m_CellularAutomata->GetRows()/2.0f;
		translation *= m_CellularAutomata->GetCellSize();
		// Min extension.
		vec3 minExtension = -translation;
		// Bounding box lower correction.
		minExtension -= vec3(m_CellularAutomata->GetCellSize()/2.0f);
		positions.push_back(minExtension);
		// Max extension.
		vec3 maxExtension = translation;
		// Bounding box upper correction.
		maxExtension -= vec3(m_CellularAutomata->GetCellSize()/2.0f);
		positions.push_back(maxExtension);
		// Creating the model matrices transform.
		for(int i = 0; i<m_CellularAutomata->GetRows(); i++)
			for(int j = 0; j<m_CellularAutomata->GetColumns(); j++)
				for(int k = 0; k<m_CellularAutomata->GetSlices(); k++) {
					Transform transform(NULL, (vec3(j, k, i)*m_CellularAutomata->GetCellSize())-translation, vec_zero, vec_one*m_CellularAutomata->GetCellSize());
					m_ModelMatrices.push_back(transform.GetWorldMatrix());
				}

		// Moving computed data to mesh vectors.
		m_Mesh = make_shared<Mesh>();
		m_Mesh->SetPositions(positions);

		// Create the bounding box for the mesh.
		m_Mesh->CreateBoundingBox();

		// Create a cube mesh.
		m_CubeMesh = unique_ptr<SubstateMesh>(new SubstateMesh());

		// Load mesh data in graphic device.
		m_CubeMesh->LoadMesh(CUBE_MESH_PATH);

		// Update mesh model matrices.
		m_CubeMesh->SetMMatrices(m_ModelMatrices);
		m_CubeMesh->UpdateMMatrices();

		// Set the ShaderSubstateMesh3D.
		m_ShaderSubstateMesh = DataManager::GetShader(SHADER_SUBSTATE_MESH_INSTANCED);
		for each (auto &material in m_Materials) {
			material->SetShader(m_ShaderSubstateMesh);
		}

		COMPUTE_EXTREMES_3D;
	}

private:
	CALModel3D*			m_Model;
	CALSubstate3Db*		m_ByteSubstate;
	CALSubstate3Di*		m_IntSubstate;
	CALSubstate3Dr*		m_RealSubstate;
	unique_ptr<Mesh>	m_CubeMesh;
	vector<mat4>		m_ModelMatrices;
	shared_ptr<Shader>	m_ShaderSubstateMesh;
};

SubstateRenderer* SubstateRenderer::DefineSubstate3D_Cube(const SubstateRendererDesc& desc) {
	if(desc.cellularAutomata->GetType()!=eCAType::CA_3D)
		return NULL; // Trying to create a 3D substate on a 2D cellular automata.
	DEFINE_SUBSTATE_RENDERER(SubstateRenderer3D_Cube);
}
