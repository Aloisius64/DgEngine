////////////////////////////////////////////////////////////////////////////////
// Filename: RandomTexture.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "RandomTexture.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


RandomTexture::RandomTexture() {
	m_Texture = 0;
}

RandomTexture::~RandomTexture() {
	DELETE_TEXTURE(m_Texture);
}

bool RandomTexture::Initialize(unsigned int size) {
	glGenTextures(1, &m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture);

	unsigned int TextureSize = size * size*3;
	float* pRandom = new float[TextureSize];
	for(unsigned int i = 0; i<TextureSize; i++) {
		float r = 2.0f * (float) rand()/RAND_MAX-1.0f;
		pRandom[i] = r;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, size, size, 0, GL_RGB, GL_FLOAT, pRandom);

	delete[] pRandom;

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);

	return dg::CheckGLError();
}

void RandomTexture::Bind(GLenum textureUnit) {
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
}
