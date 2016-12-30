////////////////////////////////////////////////////////////////////////////////
// Filename: ShadowCubeMapFBO.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "ShadowCubeMapFBO.h"
#include "Camera.h"
#include "Configuration.h"
#include "DataManager.h"
#include "Environment.h"
#include "Light.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;

#define NUM_OF_LAYERS 6

ShadowCubeMapFBO::ShadowCubeMapFBO() {
	m_Fbo = 0;
	m_ShadowMap = 0;
	m_SizeShadowMap = 0;
	m_ShadowShader = nullptr;
}

ShadowCubeMapFBO::~ShadowCubeMapFBO() {
	DELETE_FRAMEBUFFER(m_Fbo);
	DELETE_TEXTURE(m_ShadowMap);
	m_ShadowShader = nullptr;
}

bool ShadowCubeMapFBO::Initialize() {

	//glEnable(GL_TEXTURE_CUBE_MAP);

	// Get shader for generate shadow map.
	m_ShadowShader = DataManager::GetShader(SHADER_SHADOW_CUBE_MAP);

	// Get size shadow map.
	m_SizeShadowMap = Environment::Instance()->GetShadowMapSize();

	glGenFramebuffers(1, &m_Fbo);

	glGenTextures(1, &m_ShadowMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_ShadowMap);

	for(GLuint i = 0; i<6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_DEPTH_COMPONENT,
			m_SizeShadowMap, m_SizeShadowMap, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_ShadowMap, 0);
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

void ShadowCubeMapFBO::BindForWriting() const {
	glViewport(0, 0, m_SizeShadowMap, m_SizeShadowMap);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
}

void ShadowCubeMapFBO::BindForReading(GLenum TextureUnit) const {
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_ShadowMap);
}

void ShadowCubeMapFBO::Unbind() const {
	Camera::ActiveCamera()->UpdateViewport();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowCubeMapFBO::DirectionalShadowPass(Light* light) const {
	// Do nothing.
}

void ShadowCubeMapFBO::PointShadowPass(Light* light) const {
	char Name[128];
	memset(Name, 0, sizeof(Name));

	glm::vec3 lightPos = light->GetPosition();

	GLfloat fovy = 90.0f; //Camera::ActiveCamera()->GetFovy();
	GLfloat near = Camera::ActiveCamera()->GetZNear();
	GLfloat far = Camera::ActiveCamera()->GetZFar();
	glm::mat4 shadowProj = glm::perspective(fovy, 1.0f, near, far);

	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.reserve(NUM_OF_LAYERS);
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos+glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos+glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos+glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos+glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos+glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos+glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	BindForWriting();

	glClear(GL_DEPTH_BUFFER_BIT);

	m_ShadowShader->SetActive();

	for(unsigned int i = 0; i<NUM_OF_LAYERS; i++) {
		SNPRINTF(Name, sizeof(Name), "shadowTransforms[%d]", i);
		m_ShadowShader->SetFloatMatrix4(Name, shadowTransforms[i]);
	}

	m_ShadowShader->SetFloatVector3("lightPos", lightPos);
	m_ShadowShader->SetFloat("far_plane", far);

	glCullFace(GL_FRONT);
	SceneObject::Render(SceneObject::Root().get(), m_ShadowShader, true, false);
	glCullFace(GL_BACK);

	Unbind();
}

void ShadowCubeMapFBO::SpotShadowPass(Light* light) const {
	// Do nothing.
}
