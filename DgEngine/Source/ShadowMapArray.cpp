////////////////////////////////////////////////////////////////////////////////
// Filename: ShadowMapArray.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "ShadowMapArray.h"
#include "Camera.h"
#include "Environment.h"
#include "Light.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


ShadowMapArray::ShadowMapArray() {
	m_ShadowMapFbo = 0;
	m_ShadowMap = 0;
	m_ShadowMapShader = nullptr;

	m_ShadowCubeMapFbo = 0;
	m_ShadowCubeMap = 0;
	m_ShadowCubeMapShader = nullptr;
}

ShadowMapArray::~ShadowMapArray() {
	DELETE_FRAMEBUFFER(m_ShadowMapFbo);
	DELETE_TEXTURE(m_ShadowMap);

	DELETE_FRAMEBUFFER(m_ShadowCubeMapFbo);
	DELETE_TEXTURE(m_ShadowCubeMap);
}

bool ShadowMapArray::Initialize() {

	m_ShadowMapSize = Environment::Instance()->GetShadowMapSize();

	if(m_ShadowMapSize<=0)
		return false;

	m_ShadowMapShader = shared_ptr<Shader>(DataManager::GetShader(SHADER_SHADOW_MAP2));
	m_ShadowCubeMapShader = shared_ptr<Shader>(DataManager::GetShader(SHADER_SHADOW_CUBE_MAP2));

#pragma region ShadowMapFramebuffer
	glGenFramebuffers(1, &m_ShadowMapFbo);

	glGenTextures(1, &m_ShadowMap);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_ShadowMap);

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT, m_ShadowMapSize, m_ShadowMapSize, MAX_SHADOWS, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowMapFbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_ShadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion

#pragma region ShadowCubeMapFramebuffer
	glGenFramebuffers(1, &m_ShadowCubeMapFbo);
	glGenTextures(1, &m_ShadowCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, m_ShadowCubeMap);

	glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT, m_ShadowMapSize, m_ShadowMapSize, MAX_SHADOWS*6, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowCubeMapFbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_ShadowCubeMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if(Status!=GL_FRAMEBUFFER_COMPLETE) {
		dg::InfoGLFramebufferError(Status);
		return false;
	}
#pragma endregion

	return dg::CheckGLError();
}

bool ShadowMapArray::IsInitialized() const {
	return m_ShadowMapFbo!=0 && m_ShadowCubeMapFbo!=0;
}

void ShadowMapArray::BindShadowMapForWriting() const {
	glViewport(0, 0, m_ShadowMapSize, m_ShadowMapSize);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowMapFbo);
}

void ShadowMapArray::BindShadowMapForReading(GLenum textureUnit) const {
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_ShadowMap);
}

void ShadowMapArray::ShadowMapPass() const {
	glm::mat4			m_LightProjection;
	glm::mat4			m_LightView;
	vector<glm::mat4>	m_LightSpaceMatrix;

	// Directional lights view projection matrix.
	for each (auto light in DataManager::GetDirectionalLightMap()) {
		if(light.second->IsEnabled()) {
			const float projectionCube = 100.0f;
			GLfloat near_plane = -projectionCube;
			GLfloat far_plane = Camera::ActiveCamera()->GetZFar();
			m_LightProjection = glm::ortho(-projectionCube, projectionCube, -projectionCube, projectionCube, near_plane, far_plane);
			m_LightView = glm::lookAt(vec_zero, light.second->GetDirection(), vec_up);
			m_LightSpaceMatrix.push_back(m_LightProjection * m_LightView);
			light.second->SetLightSpaceMatrix(m_LightSpaceMatrix.back());
		}
	}

	// Spot lights view projection matrix.
	for each (auto light in DataManager::GetSpotLightMap()) {
		if(light.second->IsEnabled()) {
			GLfloat near_plane = Camera::ActiveCamera()->GetZNear();
			GLfloat far_plane = Camera::ActiveCamera()->GetZFar();
			m_LightProjection = glm::perspective(glm::radians(120.0f), 1.0f, near_plane, far_plane);
			m_LightView = glm::lookAt(light.second->GetPosition(), light.second->GetPosition()+light.second->GetDirection(), vec_up);
			m_LightSpaceMatrix.push_back(m_LightProjection * m_LightView);
			light.second->SetLightSpaceMatrix(m_LightSpaceMatrix.back());
		}
	}

	char Name[128];
	memset(Name, 0, sizeof(Name));

	BindShadowMapForWriting();

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	m_ShadowMapShader->SetActive();
	for(unsigned int i = 0; i<m_LightSpaceMatrix.size(); i++) {
		SNPRINTF(Name, sizeof(Name), "LightSpaceMatrix[%d]", i);
		m_ShadowMapShader->SetFloatMatrix4(Name, m_LightSpaceMatrix[i]);
	}

	glDisable(GL_CULL_FACE);
	SceneObject::Render(SceneObject::Root().get(), m_ShadowMapShader, true, false);
	glEnable(GL_CULL_FACE);

	Unbind();
}

void ShadowMapArray::BindShadowCubeMapForWriting() const {
	glViewport(0, 0, m_ShadowMapSize, m_ShadowMapSize);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowCubeMapFbo);
}

void ShadowMapArray::BindShadowCubeMapForReading(GLenum textureUnit) const {
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, m_ShadowCubeMap);
}

void ShadowMapArray::ShadowCubeMapPass() const {
	char Name[128];
	memset(Name, 0, sizeof(Name));

	GLfloat fovy = 90.0f;
	GLfloat near = Camera::ActiveCamera()->GetZNear();
	GLfloat far = Camera::ActiveCamera()->GetZFar();
	const glm::mat4 shadowProj = glm::perspective(glm::radians(fovy), 1.0f, near, far);
	std::vector<glm::mat4> shadowTransforms;

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	BindShadowCubeMapForWriting();

	glClear(GL_DEPTH_BUFFER_BIT);

	m_ShadowCubeMapShader->SetActive();

	m_ShadowCubeMapShader->SetFloat("far_plane", far);

	//glCullFace(GL_FRONT);
	glDisable(GL_CULL_FACE);
	int shadowIndex = 0;
	for each (auto light in DataManager::GetPointLightMap()) {
		if(light.second->IsEnabled()) {
			glm::vec3 lightPos = light.second->GetPosition();

			shadowTransforms.reserve(6);
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos+glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos+glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos+glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos+glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos+glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos+glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

			for(unsigned int i = 0; i<6; i++) {
				SNPRINTF(Name, sizeof(Name), "shadowTransforms[%d]", i);
				m_ShadowCubeMapShader->SetFloatMatrix4(Name, shadowTransforms[i]);
			}

			shadowTransforms.clear();

			m_ShadowCubeMapShader->SetInt("shadow", shadowIndex);
			m_ShadowCubeMapShader->SetFloatVector3("lightPos", lightPos);

			SceneObject::Render(SceneObject::Root().get(), m_ShadowCubeMapShader, true, false);

			shadowIndex++;
		}
	}
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	Unbind();
}

void ShadowMapArray::Unbind() const {
	Camera::ActiveCamera()->UpdateViewport();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
