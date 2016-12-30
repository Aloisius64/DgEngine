////////////////////////////////////////////////////////////////////////////////
// Filename: BloomFBO.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "BloomFBO.h"
#include "Configuration.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


BloomFBO::BloomFBO() {
	m_PingpongFBO[0] = 0;
	m_PingpongFBO[1] = 0;
	m_PingpongColorbuffers[0] = 0;
	m_PingpongColorbuffers[1] = 0;
	m_LastBindedBuffer = 0;
}

BloomFBO::~BloomFBO() {
	for(int i = 0; i<2; i++) {
		DELETE_FRAMEBUFFER(m_PingpongFBO[i]);
	}

	for(int i = 0; i<2; i++) {
		DELETE_TEXTURE(m_PingpongColorbuffers[i]);
	}
}

bool BloomFBO::Initialize() {

	unsigned int width = Configuration::Instance()->GetScreenWidth();
	unsigned int height = Configuration::Instance()->GetScreenHeigth();

	glGenFramebuffers(2, m_PingpongFBO);
	glGenTextures(2, m_PingpongColorbuffers);

	for(int i = 0; i<2; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, m_PingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, m_PingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // We clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_PingpongColorbuffers[i], 0);

		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if(Status!=GL_FRAMEBUFFER_COMPLETE) {
			dg::InfoGLFramebufferError(Status);
			return false;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return dg::CheckGLError();
}

void BloomFBO::BindFBO(int index) {
	glBindFramebuffer(GL_FRAMEBUFFER, m_PingpongFBO[index]);
}

void BloomFBO::BindBuffer(GLenum textureUnit, int index) {
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, m_PingpongColorbuffers[index]);
	m_LastBindedBuffer = index;
}

void BloomFBO::BindForReading(GLenum textureUnit) {
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, m_PingpongColorbuffers[m_LastBindedBuffer]);
}

void BloomFBO::Unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool BloomFBO::IsInitialized() const {
	return m_PingpongFBO[0]!=0&&
		m_PingpongFBO[1]!=0;
}
