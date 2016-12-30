////////////////////////////////////////////////////////////////////////////////
// Filename: SubstateRenderer3D_Volume.cpp
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


class SubstateRenderer3D_Volume : public SubstateRenderer {
public:
	SubstateRenderer3D_Volume(SceneObject* sceneObject, const SubstateRendererDesc& desc)
		:SubstateRenderer(sceneObject, desc) {
		SET_SUBSTATE_3D_DATA;
	}

	virtual ~SubstateRenderer3D_Volume() {
		SET_NULLPTR_TO_OPENCAL_DATA;

		DELETE_TEXTURE(m_VolumeTextureName);
		DELETE_TEXTURE(m_FrontfaceTextureName);
		DELETE_TEXTURE(m_FrontfaceDepthTextureName);
		DELETE_FRAMEBUFFER(m_FBO);

		DELETE_VERTEX_ARRAY(m_VAO);
		DELETE_BUFFER(m_VBO);
		DELETE_BUFFER(m_IBO);
	}

	virtual void Update() {
		DRAW_INFOBAR;

		if(m_DataType==eDataType::STATIC_DATA)
			return;

		COMPUTE_EXTREMES_3D;

		if((m_CellularAutomata->GetStep()-m_LastUpdateStep)>=m_CellularAutomata->GetRenderStep()) {
			m_LastUpdateStep = m_CellularAutomata->GetStep();
			// Send data from OpenCAL's substate to OpenGL Texture3D.
			UpdateDataToVolumeTexture();
		}
	}

	virtual void Render(const shared_ptr<Shader>& shader, bool instanced, const GLuint& query) {
		if(shader!=nullptr)
			return;

		//glDepthFunc(GL_LESS);
		Transform* transform = GetComponentType(GetSceneObject(), Transform);

#pragma region RENDER_FRONT_FACE
		// Framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// Frontface GLSL program
		m_ShaderFrontFace->SetActive();
		m_ShaderFrontFace->SetFloatMatrix4("MVPMatrix", transform->GetWorldViewProjectionMatrix());
		m_ShaderFrontFace->SetFloatMatrix4("MMatrix", transform->GetWorldMatrix());
		m_ShaderFrontFace->SetFloatVector3("eyePos", Camera::ActiveCamera()->GetPosition());

		// Face culling
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
		glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Restore previous frambuffer.
		glBindFramebuffer(GL_FRAMEBUFFER, DataManager::GetDefaultFramebuffer());
#pragma endregion

#pragma region RENDER_VOLUME
		{
			// Face culling
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CW);

			// Blending
			bool blend_active = Environment::Instance()->IsBlendingActive();
			Environment::Instance()->SetBlendingActive(true);
			Environment::Instance()->SetSrcBlendFunc(eBlendFunc::BF_SRC_ALPHA);
			Environment::Instance()->SetDstBlendFunc(eBlendFunc::BF_ONE_MINUS_SRC_ALPHA);

			// Volume rendering GLSL program
			m_ShaderVolume->SetActive();
			m_ShaderVolume->SetFloatMatrix4("MVPMatrix", transform->GetWorldViewProjectionMatrix());
			m_ShaderVolume->SetFloatMatrix4("MMatrix", transform->GetWorldMatrix());
			m_ShaderVolume->SetFloatMatrix4("InvMMatrix", glm::inverse(transform->GetWorldMatrix()));
			m_ShaderVolume->SetFloatVector3("eyePos", Camera::ActiveCamera()->GetPosition());
			m_ShaderVolume->SetInt("volumeTexDimension", m_CellularAutomata->GetSlices()); // Hard-coded
			m_ShaderVolume->SetInt("volumeTex", 0);
			m_ShaderVolume->SetInt("frontfaceTex", 1);
			SET_SHADER_SUBTATE_PARAMETERS(m_ShaderVolume);

			// Set needed textures
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_3D, m_VolumeTextureName);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_FrontfaceTextureName);

			// Draw!
			glBindVertexArray(m_VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
			glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			glFrontFace(GL_CCW);
			Environment::Instance()->SetBlendingActive(blend_active);
		}
#pragma endregion
	}

protected:
	virtual void Initialize() {
		SubstateRenderer::Initialize();

		// Bounding box.
		vector<vec3> positions;
		vec3 translation = vec_one*0.5f;
		//translation.x = m_CellularAutomata->GetColumns()/2.0f;
		//translation.y = m_CellularAutomata->GetSlices()/2.0f;
		//translation.z = m_CellularAutomata->GetRows()/2.0f;
		//translation *= m_CellularAutomata->GetCellSize();
		// Min extension.
		positions.push_back(-translation);
		// Max extension.
		positions.push_back(translation);

		m_Mesh = make_shared<Mesh>();
		m_Mesh->SetPositions(positions);
		m_Mesh->CreateBoundingBox();

		// Shaders initialization.
		m_ShaderFrontFace = DataManager::GetShader(SHADER_SUBSTATE_MESH_FRONTFACE);
		m_ShaderVolume = DataManager::GetShader(SHADER_SUBSTATE_MESH_VOLUME);

#pragma region VOLUME_TEXTURE
		{
			glGenTextures(1, &m_VolumeTextureName);
			glBindTexture(GL_TEXTURE_3D, m_VolumeTextureName);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(m_ColorMinValue));
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
			if(m_CodeSubstate==CODE_SUBSTATE_BYTE) {
				glTexStorage3D(GL_TEXTURE_3D, 1, GL_R8, m_CellularAutomata->GetColumns(), m_CellularAutomata->GetSlices(), m_CellularAutomata->GetRows());
			} else if(m_CodeSubstate==CODE_SUBSTATE_INT) {
				glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F, m_CellularAutomata->GetColumns(), m_CellularAutomata->GetSlices(), m_CellularAutomata->GetRows());
			} else if(m_CodeSubstate==CODE_SUBSTATE_REAL) {
				glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F, m_CellularAutomata->GetColumns(), m_CellularAutomata->GetSlices(), m_CellularAutomata->GetRows());
			}
		}
#pragma endregion

#pragma region FRONT_FACE_INFO_TEXTURE
		{
			int width = Configuration::Instance()->GetScreenWidth();
			int height = Configuration::Instance()->GetScreenHeigth();

			glGenTextures(1, &m_FrontfaceTextureName);
			glGenTextures(1, &m_FrontfaceDepthTextureName);

			glBindTexture(GL_TEXTURE_2D, m_FrontfaceTextureName);
			if(m_CodeSubstate==CODE_SUBSTATE_BYTE) {
				glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, width, height);
			} else if(m_CodeSubstate==CODE_SUBSTATE_INT) {
				glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, width, height);
			} else if(m_CodeSubstate==CODE_SUBSTATE_REAL) {
				glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, width, height);
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glBindTexture(GL_TEXTURE_2D, m_FrontfaceDepthTextureName);
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, width, height);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glGenFramebuffers(1, &m_FBO);
			glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_FrontfaceTextureName, 0);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_FrontfaceDepthTextureName, 0);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
#pragma endregion

#pragma region ARRAY_BUFFERS
		{
			vec4 const dataPositionAttribute[] = {
				// Z+
				vec4(-0.5f, -0.5f, 0.5f, 1.0f),
				vec4(0.5f, -0.5f, 0.5f, 1.0f),
				vec4(0.5f, 0.5f, 0.5f, 1.0f),
				vec4(-0.5f, 0.5f, 0.5f, 1.0f),
				// X+
				vec4(0.5f, -0.5f, 0.5f, 1.0f),
				vec4(0.5f, -0.5f, -0.5f, 1.0f),
				vec4(0.5f, 0.5f, -0.5f, 1.0f),
				vec4(0.5f, 0.5f, 0.5f, 1.0f),
				// Z-
				vec4(-0.5f, -0.5f, -0.5f, 1.0f),
				vec4(-0.5f, 0.5f, -0.5f, 1.0f),
				vec4(0.5f, 0.5f, -0.5f, 1.0f),
				vec4(0.5f, -0.5f, -0.5f, 1.0f),
				// X-
				vec4(-0.5f, -0.5f, 0.5f, 1.0f),
				vec4(-0.5f, 0.5f, 0.5f, 1.0f),
				vec4(-0.5f, 0.5f, -0.5f, 1.0f),
				vec4(-0.5f, -0.5f, -0.5f, 1.0f),
				// Y-
				vec4(0.5f, -0.5f, 0.5f, 1.0f),
				vec4(-0.5f, -0.5f, 0.5f, 1.0f),
				vec4(-0.5f, -0.5f, -0.5f, 1.0f),
				vec4(0.5f, -0.5f, -0.5f, 1.0f),
				// Y+
				vec4(0.5f, 0.5f, 0.5f, 1.0f),
				vec4(0.5f, 0.5f, -0.5f, 1.0f),
				vec4(-0.5f, 0.5f, -0.5f, 1.0f),
				vec4(-0.5f, 0.5f, 0.5f, 1.0f),
			};

			GLsizeiptr const sizePositionAttributeData = sizeof(dataPositionAttribute);

			glGenVertexArrays(1, &m_VAO);
			glBindVertexArray(m_VAO);

			glGenBuffers(1, &m_VBO);
			glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
			glBufferData(GL_ARRAY_BUFFER, sizePositionAttributeData, &dataPositionAttribute[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			// Fill our index buffer
			unsigned int const dataIndices[] = {
				0, 1, 2,
				2, 3, 0,

				4, 5, 6,
				6, 7, 4,

				8, 9, 10,
				10, 11, 8,

				12, 13, 14,
				14, 15, 12,

				16, 17, 18,
				18, 19, 16,

				20, 21, 22,
				22, 23, 20,
			};

			GLsizeiptr sizeIndexData = sizeof(dataIndices);
			m_IndexCount = sizeIndexData/sizeof(dataIndices[0]);

			// Create our gl buffer that will be used as an element buffer
			glGenBuffers(1, &m_IBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndexData, &dataIndices[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

			glEnableVertexAttribArray(0);

			glBindVertexArray(0);
		}
#pragma endregion

		UpdateDataToVolumeTexture();

		COMPUTE_EXTREMES_3D;
	}

	void UpdateDataToVolumeTexture() {
		glBindTexture(GL_TEXTURE_3D, m_VolumeTextureName);
		if(m_CodeSubstate==CODE_SUBSTATE_BYTE) {
			glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, m_CellularAutomata->GetColumns(), m_CellularAutomata->GetSlices(), m_CellularAutomata->GetRows(), GL_RED, GL_BYTE, m_ByteSubstate->current);
		} else if(m_CodeSubstate==CODE_SUBSTATE_INT) {
			int dimension = m_CellularAutomata->GetRows()*m_CellularAutomata->GetColumns()*m_CellularAutomata->GetSlices();
			float* tmp = new float[dimension];
			for(int i = 0; i<dimension; i++) {
				tmp[i] = (float) m_IntSubstate->current[i];
			}
			glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, m_CellularAutomata->GetColumns(), m_CellularAutomata->GetSlices(), m_CellularAutomata->GetRows(), GL_RED, GL_FLOAT, tmp);
			delete tmp;
		} else if(m_CodeSubstate==CODE_SUBSTATE_REAL) {
			int dimension = m_CellularAutomata->GetRows()*m_CellularAutomata->GetColumns()*m_CellularAutomata->GetSlices();
			float* tmp = new float[dimension];
			for(int i = 0; i<dimension; i++) {
				tmp[i] = (float) m_RealSubstate->current[i];
			}
			glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, m_CellularAutomata->GetColumns(), m_CellularAutomata->GetSlices(), m_CellularAutomata->GetRows(), GL_RED, GL_FLOAT, tmp);
			delete tmp;
		}
		glBindTexture(GL_TEXTURE_3D, 0);
	}

private:
	CALModel3D*			m_Model;
	CALSubstate3Db*		m_ByteSubstate;
	CALSubstate3Di*		m_IntSubstate;
	CALSubstate3Dr*		m_RealSubstate;
	shared_ptr<Shader>	m_ShaderFrontFace;
	shared_ptr<Shader>	m_ShaderVolume;
	GLuint				m_VolumeTextureName;
	GLuint				m_FBO;
	GLuint				m_FrontfaceTextureName;
	GLuint				m_FrontfaceDepthTextureName;
	GLuint				m_VAO;
	GLuint				m_VBO;
	GLuint				m_IBO;
	GLsizei				m_IndexCount;
};

SubstateRenderer* SubstateRenderer::DefineSubstate3D_Volume(const SubstateRendererDesc& desc) {
	if(desc.cellularAutomata->GetType()!=eCAType::CA_3D)
		return NULL; // Trying to create a 3D substate on a 2D cellular automata.
	DEFINE_SUBSTATE_RENDERER(SubstateRenderer3D_Volume);
}
