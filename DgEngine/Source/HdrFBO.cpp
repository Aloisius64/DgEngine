////////////////////////////////////////////////////////////////////////////////
// Filename: HdrFBO.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "HdrFBO.h"
#include "Configuration.h"
#include "DataManager.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


HdrFBO::HdrFBO() {
	m_Fbo = 0;
	m_ColorBuffer = 0;
	m_RboDepth = 0;
}

HdrFBO::~HdrFBO() {
	DELETE_FRAMEBUFFER(m_Fbo);
	DELETE_TEXTURE(m_ColorBuffer);
	DELETE_TEXTURE(m_BloomBuffer);
	DELETE_TEXTURE(m_RboDepth);
}

bool HdrFBO::Initialize() {

	unsigned int width = Configuration::Instance()->GetScreenWidth();
	unsigned int height = Configuration::Instance()->GetScreenHeigth();

	// Set up floating point framebuffer to render scene to.
	glGenFramebuffers(1, &m_Fbo);

	// Create floating point color buffer.
	glGenTextures(1, &m_ColorBuffer);
	glBindTexture(GL_TEXTURE_2D, m_ColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Create floating point bloom buffer
	glGenTextures(1, &m_BloomBuffer);
	glBindTexture(GL_TEXTURE_2D, m_BloomBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// We clamp to the edge as the blur filter would otherwise sample repeated texture values!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Create depth buffer (renderbuffer).
	glGenRenderbuffers(1, &m_RboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	
	// Attach buffers
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorBuffer, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_BloomBuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RboDepth);

	GLuint attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, attachments);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if(Status!=GL_FRAMEBUFFER_COMPLETE) {
		dg::InfoGLFramebufferError(Status);
		return false;
	}

	return dg::CheckGLError();
}

void HdrFBO::BindForWriting() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
}

void HdrFBO::BindForReading(GLenum textureUnit) {
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, m_ColorBuffer);
}

void HdrFBO::BindBloomBuffer(GLenum textureUnit) {
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, m_BloomBuffer);
}

void HdrFBO::Unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, DataManager::m_DefaultFramebuffer);
}

bool HdrFBO::IsInitialized() const {
	return m_Fbo!=0;
}
