////////////////////////////////////////////////////////////////////////////////
// Filename: SubstateRenderer2D_Flat.cpp
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
#include "Camera.h"
#include "Configuration.h"
#include "DataManager.h"
#include "Environment.h"
#include "Glossary.h"
#include "Mesh.h"
#include "SubstateMesh.h"
#include "Transform.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


class SubstateRenderer2D_Flat : public SubstateRenderer {
public:
	SubstateRenderer2D_Flat(SceneObject* sceneObject, const SubstateRendererDesc& desc)
		:SubstateRenderer(sceneObject, desc) {
		SET_SUBSTATE_2D_DATA;
		m_PlaneMesh = nullptr;
	}

	virtual ~SubstateRenderer2D_Flat() {
		SET_NULLPTR_TO_OPENCAL_DATA;
		m_PlaneMesh = nullptr;
	}

	virtual void Update() {
		DRAW_INFOBAR;

		if(m_DataType==eDataType::STATIC_DATA)
			return;

		COMPUTE_EXTREMES_2D;

		if((m_CellularAutomata->GetStep()-m_LastUpdateStep)>=m_CellularAutomata->GetRenderStep()) {
			m_LastUpdateStep = m_CellularAutomata->GetStep();
			int dimension = m_CellularAutomata->GetRows()*m_CellularAutomata->GetColumns()*m_CellularAutomata->GetSlices();
			if(m_CodeSubstate==CODE_SUBSTATE_BYTE) {
				((SubstateMesh*) m_PlaneMesh.get())->UpdateByteData(m_ByteSubstate->current, dimension);
			} else if(m_CodeSubstate==CODE_SUBSTATE_INT) {
				((SubstateMesh*) m_PlaneMesh.get())->UpdateIntData(m_IntSubstate->current, dimension);
			} else if(m_CodeSubstate==CODE_SUBSTATE_REAL) {
				((SubstateMesh*) m_PlaneMesh.get())->UpdateRealData(m_RealSubstate->current, dimension);
			}
		}
	}

	virtual void Render(const shared_ptr<Shader>& shader, bool instanced, const GLuint& query) {
		if(shader!=nullptr)
			return;

		Camera::ActiveCamera()->SetProjection(eProjection::ORTHOGRAPHIC_PROJECTION);

		// Set parameters to the shader.
		SetShaderParameters(shader, true);

		m_ShaderSubstateMesh->SetActive();
		SET_SHADER_SUBTATE_PARAMETERS(m_ShaderSubstateMesh);

		m_PlaneMesh->RenderInstanced(NULL_QUERY, shader ? NULL : &m_Materials);

		Camera::ActiveCamera()->SetProjection(eProjection::PERSPECTIVE_PROJECTION);
	}

protected:
	virtual void Initialize() {
		SubstateRenderer::Initialize();

		// Computing the data for OpenGL buffers.
		vector<vec3> positions;
		vec3 translation;
		translation.x = m_CellularAutomata->GetColumns()/2.0f;
		translation.y = 0.0f;
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
			for(int j = 0; j<m_CellularAutomata->GetColumns(); j++) {
				Transform transform(NULL, (vec3(j, 0.0f, i)*m_CellularAutomata->GetCellSize())-translation, vec_zero, vec_one*m_CellularAutomata->GetCellSize());
				m_ModelMatrices.push_back(transform.GetWorldMatrix());
			}

		// Moving computed data to mesh vectors.
		m_Mesh = make_shared<Mesh>();
		m_Mesh->SetPositions(positions);

		// Create the bounding box for the mesh.
		m_Mesh->CreateBoundingBox();

		// Create a cube mesh.
		m_PlaneMesh = unique_ptr<SubstateMesh>(new SubstateMesh());

		// Load mesh data in graphic device.
		m_PlaneMesh->LoadMesh(PLANE_MESH_PATH);

		// Update mesh model matrices.
		m_PlaneMesh->SetMMatrices(m_ModelMatrices);
		m_PlaneMesh->UpdateMMatrices();

		// Set the ShaderSubstateMesh2D.
		m_ShaderSubstateMesh = DataManager::GetShader(SHADER_SUBSTATE_MESH_INSTANCED);
		for each (auto &material in m_Materials) {
			material->SetShader(m_ShaderSubstateMesh);
		}

		COMPUTE_EXTREMES_2D;
	}

private:
	CALModel2D*			m_Model;
	CALSubstate2Db*		m_ByteSubstate;
	CALSubstate2Di*		m_IntSubstate;
	CALSubstate2Dr*		m_RealSubstate;
	unique_ptr<Mesh>	m_PlaneMesh;
	vector<mat4>		m_ModelMatrices;
	shared_ptr<Shader>	m_ShaderSubstateMesh;
};

SubstateRenderer* SubstateRenderer::DefineSubstate2D_Flat(const SubstateRendererDesc& desc) {
	if(desc.cellularAutomata->GetType()!=eCAType::CA_2D)
		return NULL; // Trying to create a 2D substate on a 2D cellular automata.
	DEFINE_SUBSTATE_RENDERER(SubstateRenderer2D_Flat);
}
