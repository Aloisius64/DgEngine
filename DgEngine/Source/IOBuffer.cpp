////////////////////////////////////////////////////////////////////////////////
// Filename: IOBuffer.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "IOBuffer.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


IOBuffer::IOBuffer() {
	m_Fbo = 0;
	m_Texture = 0;
	m_Depth = 0;
	m_InternalType = GL_NONE;
}

IOBuffer::~IOBuffer() {
	DELETE_FRAMEBUFFER(m_Fbo);
	DELETE_TEXTURE(m_Texture);
	DELETE_TEXTURE(m_Depth);
}

bool IOBuffer::Initialize(unsigned int width, unsigned int height, bool depth, GLenum internalType) {
	m_InternalType = internalType;

	GLenum format, type;

	switch(internalType) {
	case GL_RGB32F:
		format = GL_RGB;
		type = GL_FLOAT;
		break;
	case GL_R32F:
		format = GL_RED;
		type = GL_FLOAT;
		break;
	case GL_NONE:
		break;
	default:
		dg::DebugConsoleMessage("Invalid internal type");
	}

	// Create the FBO
	glGenFramebuffers(1, &m_Fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);

	// Create the textures
	if(m_InternalType!=GL_NONE) {
		glGenTextures(1, &m_Texture);

		glBindTexture(GL_TEXTURE_2D, m_Texture);
		glTexImage2D(GL_TEXTURE_2D, 0, m_InternalType, width, height, 0, format, type, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture, 0);

		GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT0};

		//glDrawBuffers(ARRAY_SIZE_IN_ELEMENTS(DrawBuffers), DrawBuffers);
		for each (GLenum buffer in DrawBuffers) {
			glDrawBuffer(buffer);
		}
	}

	// Create the depth buffer 
	if(depth) {
		glGenTextures(1, &m_Depth);

		// depth
		glBindTexture(GL_TEXTURE_2D, depth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);
	}

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if(Status!=GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
		return false;
	}

	// restore default FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	return true;
}

void IOBuffer::BindForWriting() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
}

void IOBuffer::BindForReading(GLenum textureUnit) {
	glActiveTexture(textureUnit);

	if(m_InternalType==GL_NONE) {
		glBindTexture(GL_TEXTURE_2D, m_Depth);
	} else {
		glBindTexture(GL_TEXTURE_2D, m_Texture);
	}
}
