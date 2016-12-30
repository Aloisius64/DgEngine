////////////////////////////////////////////////////////////////////////////////
// Filename: ShadowMapFBO.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "ShadowMapFBO.h"
#include "Camera.h"
#include "Configuration.h"
#include "DataManager.h"
#include "Environment.h"
#include "Light.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;

ShadowMapFBO::ShadowMapFBO() {
	m_Fbo = 0;
	m_ShadowMap = 0;
	m_SizeShadowMap = 0;
	m_ShadowShader = nullptr;
}

ShadowMapFBO::~ShadowMapFBO() {
	DELETE_FRAMEBUFFER(m_Fbo);
	DELETE_TEXTURE(m_ShadowMap);
	m_ShadowShader = nullptr;
}

bool ShadowMapFBO::Initialize() {

	// Get shader for generate shadow map.
	m_ShadowShader = shared_ptr<Shader>(DataManager::GetShader(SHADER_SHADOW_MAP));

	// Get size shadow map.
	m_SizeShadowMap = Environment::Instance()->GetShadowMapSize();
	
	glGenFramebuffers(1, &m_Fbo);
	glGenTextures(1, &m_ShadowMap);
	glBindTexture(GL_TEXTURE_2D, m_ShadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_SizeShadowMap, m_SizeShadowMap, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = {1.0, 1.0, 1.0, 1.0};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if(Status!=GL_FRAMEBUFFER_COMPLETE) {
		dg::InfoGLFramebufferError(Status);
		return false;
	}

	return dg::CheckGLError();
}

void ShadowMapFBO::BindForWriting() const {
	glViewport(0, 0, m_SizeShadowMap, m_SizeShadowMap);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
}

void ShadowMapFBO::BindForReading(GLenum TextureUnit) const {
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_2D, m_ShadowMap);
}

void ShadowMapFBO::Unbind() const {
	Camera::ActiveCamera()->UpdateViewport();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMapFBO::DirectionalShadowPass(Light* light) const {
	glm::mat4 lightProjection;
	glm::mat4 lightView;
	glm::mat4 lightSpaceMatrix;
	const float projectionCube = 100.0f;
	GLfloat near_plane = -projectionCube; // -10.0f // Camera::ActiveCamera()->GetZNear();
	GLfloat far_plane = Camera::ActiveCamera()->GetZFar();
	lightProjection = glm::ortho(-projectionCube, projectionCube, -projectionCube, projectionCube, near_plane, far_plane);
	lightView = glm::lookAt(vec_zero, light->GetDirection(), vec_up);
	lightSpaceMatrix = lightProjection * lightView;

	// Render scene from light's point of view.
	m_ShadowShader->SetActive();
	m_ShadowShader->SetFloatMatrix4("LightSpaceMatrix", lightSpaceMatrix);
	light->SetLightSpaceMatrix(lightSpaceMatrix);

	BindForWriting();
	glClear(GL_DEPTH_BUFFER_BIT);

	glDisable(GL_CULL_FACE);
	SceneObject::Render(SceneObject::Root().get(), m_ShadowShader, true, false);
	glEnable(GL_CULL_FACE);
	
	Unbind();
}

void ShadowMapFBO::PointShadowPass(Light* light) const {
	// Do nothing.
}

void ShadowMapFBO::SpotShadowPass(Light* light) const {
	glm::mat4 lightProjection;
	glm::mat4 lightView;
	glm::mat4 lightSpaceMatrix;
	GLfloat near_plane = Camera::ActiveCamera()->GetZNear();
	GLfloat far_plane = Camera::ActiveCamera()->GetZFar();
	lightProjection = glm::perspective(90.0f, 1.0f, near_plane, far_plane);
	lightView = glm::lookAt(light->GetPosition(), light->GetPosition()+light->GetDirection(), vec_up);
	lightSpaceMatrix = lightProjection * lightView;

	// Render scene from light's point of view.
	m_ShadowShader->SetActive();
	m_ShadowShader->SetFloatMatrix4("LightSpaceMatrix", lightSpaceMatrix);
	light->SetLightSpaceMatrix(lightSpaceMatrix);

	BindForWriting();
	glClear(GL_DEPTH_BUFFER_BIT);

	glDisable(GL_CULL_FACE);
	SceneObject::Render(SceneObject::Root().get(), m_ShadowShader, true, false);
	glEnable(GL_CULL_FACE);
	
	Unbind();
}
