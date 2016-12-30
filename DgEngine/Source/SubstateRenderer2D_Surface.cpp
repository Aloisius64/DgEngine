////////////////////////////////////////////////////////////////////////////////
// Filename: SubstateRenderer2D_Surface.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////
#include <AntTweakBar.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "SubstateRenderer.h"
#include "Behaviour.h"
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


#define ComputeIndex(columns, i, j) ((((i)*(columns)) + (j)))
#define PRIMITIVE_RESTART_INDEX 0xFFFF
#define SIZE_X 50.0f
#define SIZE_Y 50.0f
#define SIZE_Z 50.0f

class SubstateRenderer2D_Surface : public SubstateRenderer {
public:
	SubstateRenderer2D_Surface(SceneObject* sceneObject, const SubstateRendererDesc& desc)
		:SubstateRenderer(sceneObject, desc) {
		SET_SUBSTATE_2D_DATA;
		// Helper thread data.
		m_StopThread = false;
	}

	virtual ~SubstateRenderer2D_Surface() {
		if(m_DataType==eDataType::DYNAMIC_DATA) {
			{
				std::unique_lock<std::mutex> lock(m_Mutex);
				m_StopThread = true;
			}
			m_DataProcessingThread.join();
		}

		SET_NULLPTR_TO_OPENCAL_DATA;
	}

	virtual void Update() {
		DRAW_INFOBAR;

		// We have to update only dynamic substate.
		// Static substates remains the same for all computation.
		if(m_DataType==eDataType::STATIC_DATA)
			return;

		COMPUTE_EXTREMES_2D;

		if((m_CellularAutomata->GetStep()-m_LastUpdateStep)>=m_CellularAutomata->GetRenderStep()) {
			m_LastUpdateStep = m_CellularAutomata->GetStep();
			{	// Lock
				std::unique_lock<std::mutex> lock(m_Mutex);
				// Update OpenGL buffers.
				m_Mesh->UpdatePositions();
				m_Mesh->UpdateNormals();
				m_Mesh->UpdateIndices();

				int dimension = m_CellularAutomata->GetRows()*m_CellularAutomata->GetColumns()*m_CellularAutomata->GetSlices();
				if(m_CodeSubstate==CODE_SUBSTATE_BYTE) {
					((SubstateMesh*) m_Mesh.get())->UpdateByteData(m_ByteSubstate->current, dimension);
				} else if(m_CodeSubstate==CODE_SUBSTATE_INT) {
					((SubstateMesh*) m_Mesh.get())->UpdateIntData(m_IntSubstate->current, dimension);
				} else if(m_CodeSubstate==CODE_SUBSTATE_REAL) {
					((SubstateMesh*) m_Mesh.get())->UpdateRealData(m_RealSubstate->current, dimension);
				}
			}	// Unlock
		}
	}

	virtual void Render(const shared_ptr<Shader>& shader, bool instanced, const GLuint& query) {
		if(shader!=nullptr)
			return;

		glEnable(GL_PRIMITIVE_RESTART);
		RenderSubstates(GetSceneObject(), shader);
		glDisable(GL_PRIMITIVE_RESTART);
	}

	void SetShaderParameters(const shared_ptr<Shader>& shader, bool instanced) {
		static shared_ptr<Shader> lastShader = nullptr;

		// If there is an external shader use it,
		// otherwise use the material shader.
		if(shader) {
			lastShader = shader;
		} else {
			// If all materials share the same shader,
			// I set the SceneObject values only one time.
			for each (auto material in m_Materials) {
				if(material->GetShader()!=lastShader) {
					lastShader = material->GetShader();
				}
				lastShader->SetActive();
			}

			SET_SHADER_SUBTATE_PARAMETERS(lastShader);

			lastShader->SetFloatVector3("PreTranslation", m_Translation);
			lastShader->SetFloatVector3("PreScaling", m_Scaling);
		}

		// Set if shadeless or not.
		lastShader->SetInt("Shadeless", m_Shadeless);
		lastShader->SetParameters(GetSceneObject());

		dg::CheckGLError();
	}

protected:
	virtual void Initialize() {
		SubstateRenderer::Initialize();

		m_Mesh = make_shared<SubstateMesh>(m_DataType, false);

		vector<vec3> positions;
		vector<vec2> texCoords;
		vector<vec3> normals;
		vector<vec3> tangents;
		vector<unsigned int> indices;

		unsigned int numVertices = m_CellularAutomata->GetRows()*m_CellularAutomata->GetColumns();
		positions.reserve(numVertices);
		normals.reserve(numVertices);
		texCoords.reserve(numVertices);
		tangents.reserve(numVertices);

		// Computing the data for OpenGL buffers.
		ComputeData(&positions, &texCoords, &normals, &tangents, &indices);

		// Set pre-translation values.
		m_Translation.x = (m_CellularAutomata->GetColumns() * m_CellularAutomata->GetCellSize())/2.0f;
		m_Translation.y = (float) (m_MaxHeight-m_MinHeight)/2.0f;
		m_Translation.z = (m_CellularAutomata->GetRows() * m_CellularAutomata->GetCellSize())/2.0f;

		// Set pre-scaling values.
		m_Scaling.x = SIZE_X/(m_CellularAutomata->GetColumns());
		m_Scaling.y = SIZE_Y/(float) (m_MaxHeight-m_MinHeight);
		m_Scaling.z = SIZE_Z/(m_CellularAutomata->GetRows());

		// Moving computed data to mesh vectors.
		m_Mesh->SetPositions(positions);
		m_Mesh->SetTexCoords(texCoords);
		m_Mesh->SetNormals(normals);
		m_Mesh->SetTangents(tangents);
		m_Mesh->SetIndices(indices);

		if(m_Mesh->CreateMesh()) {
			// Create the bounding box centered to origin.
			vec3 minExt = (m_Mesh->GetMinExtension()-m_Translation)*m_Scaling;
			vec3 maxExt = (m_Mesh->GetMaxExtension()-m_Translation)*m_Scaling;
			m_Mesh->CreateBoundingBox(minExt, maxExt);

			if(m_DataType==eDataType::DYNAMIC_DATA) {
				m_DataProcessingThread = thread(
					[this] {
					bool exit = false;
					vector<vec3> positions;
					vector<vec2> texCoords;
					vector<vec3> normals;
					vector<vec3> tangents;
					vector<unsigned int> indices;

					while(!exit) {
						{	// Lock
							std::unique_lock<std::mutex> lock(m_Mutex);
							exit = m_StopThread||m_CellularAutomata==NULL;
						}	// Unlock

						if(!exit) {
							positions.clear();
							normals.clear();
							texCoords.clear();
							tangents.clear();

							unsigned int numVertices = m_CellularAutomata->GetRows()*m_CellularAutomata->GetColumns();
							positions.reserve(numVertices);
							normals.reserve(numVertices);
							texCoords.reserve(numVertices);
							tangents.reserve(numVertices);

							// Computing the data for OpenGL buffers.
							ComputeData(&positions, &texCoords, &normals, &tangents, &indices);

							// Set pre-translation values.
							m_Translation.x = (m_CellularAutomata->GetColumns() * m_CellularAutomata->GetCellSize())/2.0f;
							m_Translation.y = (float) (m_MaxHeight-m_MinHeight)/2.0f;
							m_Translation.z = (m_CellularAutomata->GetRows() * m_CellularAutomata->GetCellSize())/2.0f;

							// Set pre-scaling values.
							m_Scaling.x = SIZE_X/(m_CellularAutomata->GetColumns());
							m_Scaling.y = SIZE_Y/(float) (m_MaxHeight-m_MinHeight);
							m_Scaling.z = SIZE_Z/(m_CellularAutomata->GetRows());
						
							{	// Lock
								std::unique_lock<std::mutex> lock(m_Mutex);
								// Moving computed data to mesh vectors.
								m_Mesh->SetPositions(positions);
								m_Mesh->SetNormals(normals);
								m_Mesh->SetIndices(indices);

								m_Mesh->UpdatePositions();
								m_Mesh->UpdateNormals();
								m_Mesh->UpdateIndices();
							}	// Unlock
						}
					}
				}
				);
			}
		}

		// Set the ShaderSubstateMesh2D.
		m_ShaderSubstateMesh = DataManager::GetShader(SHADER_SUBSTATE_MESH_SURFACE);
		for each (auto &material in m_Materials) {
			material->SetShader(m_ShaderSubstateMesh);
		}

		COMPUTE_EXTREMES_2D;

		int dimension = m_CellularAutomata->GetRows()*m_CellularAutomata->GetColumns()*m_CellularAutomata->GetSlices();
		if(m_CodeSubstate==CODE_SUBSTATE_BYTE) {
			((SubstateMesh*) m_Mesh.get())->UpdateByteData(m_ByteSubstate->current, dimension);
		} else if(m_CodeSubstate==CODE_SUBSTATE_INT) {
			((SubstateMesh*) m_Mesh.get())->UpdateIntData(m_IntSubstate->current, dimension);
		} else if(m_CodeSubstate==CODE_SUBSTATE_REAL) {
			((SubstateMesh*) m_Mesh.get())->UpdateRealData(m_RealSubstate->current, dimension);
		}
	}

	void ComputeData(vector<vec3>* positions, vector<vec2>* texCoords, vector<vec3>* normals, vector<vec3>* tangents, vector<unsigned int>* indices) {
		// Vertex processing.
		for(int i = 0; i<m_CellularAutomata->GetRows(); i++) {
			for(int j = 0; j<m_CellularAutomata->GetColumns(); j++) {
				// Position.
				if(positions) {
					vec3 position;
					position.x = j * m_CellularAutomata->GetCellSize();
					position.y = (float) ComputeVertex(i, j);
					position.z = i * m_CellularAutomata->GetCellSize();
					positions->push_back(position);
				}
				// TexCoord.
				if(texCoords) {
					vec2 texCoord(0.0f, 0.0f);
					texCoord.s = j/(float) (m_CellularAutomata->GetColumns()-1);;
					texCoord.t = ((m_CellularAutomata->GetRows()-1)-(i+1))/(float) ((m_CellularAutomata->GetRows()-1)-1);
					texCoords->push_back(texCoord);
				}
				// Normal.
				if(normals) {
					vec3 normal = vec_up;
					if(j<m_CellularAutomata->GetRows()-1
						&&i<m_CellularAutomata->GetColumns()-1) {
						vec3 points[3];
						points[0][0] = j * m_CellularAutomata->GetCellSize();
						points[0][1] = 1.0f;
						if(m_CodeSubstate==CODE_SUBSTATE_BYTE) {
							points[0][1] = (float) calGet2Db(m_Model, m_ByteSubstate, i, j);
						} else if(m_CodeSubstate==CODE_SUBSTATE_INT) {
							points[0][1] = (float) calGet2Di(m_Model, m_IntSubstate, i, j);
						} else if(m_CodeSubstate==CODE_SUBSTATE_REAL) {
							points[0][1] = (float) calGet2Dr(m_Model, m_RealSubstate, i, j);
						}
						points[0][2] = i * m_CellularAutomata->GetCellSize();

						points[1][0] = j * m_CellularAutomata->GetCellSize();
						points[1][1] = 1.0f;
						if(m_CodeSubstate==CODE_SUBSTATE_BYTE) {
							points[1][1] = (float) calGet2Db(m_Model, m_ByteSubstate, i+1, j);
						} else if(m_CodeSubstate==CODE_SUBSTATE_INT) {
							points[1][1] = (float) calGet2Di(m_Model, m_IntSubstate, i+1, j);
						} else if(m_CodeSubstate==CODE_SUBSTATE_REAL) {
							points[1][1] = (float) calGet2Dr(m_Model, m_RealSubstate, i+1, j);
						}
						points[1][2] = (i+1) * m_CellularAutomata->GetCellSize();

						points[2][0] = (j+1) * m_CellularAutomata->GetCellSize();
						points[2][1] = 1.0f;
						if(m_CodeSubstate==CODE_SUBSTATE_BYTE) {
							points[2][1] = (float) calGet2Db(m_Model, m_ByteSubstate, i, j+1);
						} else if(m_CodeSubstate==CODE_SUBSTATE_INT) {
							points[2][1] = (float) calGet2Di(m_Model, m_IntSubstate, i, j+1);
						} else if(m_CodeSubstate==CODE_SUBSTATE_REAL) {
							points[2][1] = (float) calGet2Dr(m_Model, m_RealSubstate, i, j+1);
						}
						points[2][2] = i * m_CellularAutomata->GetCellSize();

						vec3 v1 = points[1]-points[0];
						vec3 v2 = points[2]-points[0];

						normal = normalize(cross(v1, v2));
					}
					normals->push_back(normal);
				}
				// Tangent.
				if(tangents) {
					// TODO Tangents computing missing
					tangents->push_back(vec_zero);
				}
			}
		}

		// Computing max and min values.
		ComputeExtremes();

		// Index processing.
		if(indices) {
			CALreal noDataValue = 0.0;
			if(m_CodeSubstate==CODE_SUBSTATE_BYTE) {
				noDataValue = (CALreal) m_ByteNoDataValue;
			} else if(m_CodeSubstate==CODE_SUBSTATE_INT) {
				noDataValue = (CALreal) m_IntNoDataValue;
			} else if(m_CodeSubstate==CODE_SUBSTATE_REAL) {
				noDataValue = m_RealNoDataValue;
			}

			// Specify the restart primitive index.
			glPrimitiveRestartIndex((unsigned int) noDataValue);
			for(int i = 0; i<m_CellularAutomata->GetRows()-1; i++) {
				for(int j = 0; j<m_CellularAutomata->GetColumns(); j++) {
					if(ComputeVertex(i, j)!=noDataValue
						&&ComputeVertex(i+1, j)!=noDataValue) {
						indices->push_back(ComputeIndex(m_CellularAutomata->GetColumns(), i, j));
						indices->push_back(ComputeIndex(m_CellularAutomata->GetColumns(), i+1, j));
					}
				}
				indices->push_back((unsigned int) noDataValue);
			}
		}
	}

	void RenderSubstates(SceneObject* sceneObject, const shared_ptr<Shader>& shader) {
		if(sceneObject) {
			SubstateRenderer* substateRenderer = GetComponentType(sceneObject, SubstateRenderer);
			// Set parameters to the shader.
			substateRenderer->SetShaderParameters(shader, false);
			//substateRenderer->GetMesh()->Render(NULL_QUERY, shader==nullptr ? &substateRenderer->GetMaterials() : NULL, GL_TRIANGLES);
			substateRenderer->GetMesh()->Render(NULL_QUERY, shader==nullptr ? &substateRenderer->GetMaterials() : NULL, GL_TRIANGLE_STRIP);
			for each (auto child in sceneObject->Children()) {
				RenderSubstates(child, shader);
			}
		}
	}

private:
	virtual void ComputeExtremes() {
		m_MinHeight = ComputeVertex(0, 0);
		m_MaxHeight = ComputeVertex(0, 0);
		for(int i = 0; i<m_CellularAutomata->GetRows(); i++) {
			for(int j = 0; j<m_CellularAutomata->GetColumns(); j++) {
				CALreal tmp = ComputeVertex(i, j);
				if(tmp<m_MinHeight) {
					m_MinHeight = tmp;
				}
				if(tmp>m_MaxHeight) {
					m_MaxHeight = tmp;
				}
			}
		}
	}

	CALreal ComputeVertex(int i, int j) {
		CALreal tmp = 0.0;
		SceneObject* currentObject = GetSceneObject();

		SubstateRenderer2D_Surface* parentRenderer = nullptr;
		int depth = 0;
		do {
			if(currentObject->GetComponent(eComponentType::MESH_RENDERER)) {
				parentRenderer = dynamic_cast<SubstateRenderer2D_Surface*>(currentObject->GetComponent(eComponentType::MESH_RENDERER));
				if(parentRenderer) {
					if(m_CodeSubstate==CODE_SUBSTATE_BYTE) {
						tmp += (CALreal) calGet2Db(m_Model, parentRenderer->m_ByteSubstate, i, j);
					} else if(m_CodeSubstate==CODE_SUBSTATE_INT) {
						tmp += (CALreal) calGet2Di(m_Model, parentRenderer->m_IntSubstate, i, j);
					} else if(m_CodeSubstate==CODE_SUBSTATE_REAL) {
						float offset = 0.0f;
						if(calGet2Dr(m_Model, parentRenderer->m_RealSubstate, i, j!=m_RealNoDataValue)) {
							offset += 10.0f;
						}
						tmp += (offset+calGet2Dr(m_Model, parentRenderer->m_RealSubstate, i, j));
					}
				}
				currentObject = currentObject->Parent();
			} else {
				currentObject = nullptr;
			}
			depth++;
		} while(currentObject);

		return tmp;
	}

private:
	CALModel2D*			m_Model;
	CALSubstate2Db*		m_ByteSubstate;
	CALSubstate2Di*		m_IntSubstate;
	CALSubstate2Dr*		m_RealSubstate;
	// Heights.
	CALreal				m_MinHeight;
	CALreal				m_MaxHeight;
	shared_ptr<Shader>	m_ShaderSubstateMesh;
	// Helper vertex computing thread data.
	thread				m_DataProcessingThread;
	mutex				m_Mutex;
	bool				m_StopThread;
};

SubstateRenderer* SubstateRenderer::DefineSubstate2D_Surface(const SubstateRendererDesc& desc) {
	if(desc.cellularAutomata->GetType()!=eCAType::CA_2D)
		return NULL; // Trying to create a 2D substate on a 3D cellular automata.
	DEFINE_SUBSTATE_RENDERER(SubstateRenderer2D_Surface);
}
