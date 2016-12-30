////////////////////////////////////////////////////////////////////////////////
// Filename: CubemapTexture.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "CubemapTexture.h"
#include "ImageLoader.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


static const GLenum types[6] = {
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

CubemapTexture::CubemapTexture(const string& PosXFilename,
	const string& NegXFilename,
	const string& PosYFilename,
	const string& NegYFilename,
	const string& PosZFilename,
	const string& NegZFilename) {

	m_FileNames.reserve(6);
	m_FileNames.push_back(PosXFilename);
	m_FileNames.push_back(NegXFilename);
	m_FileNames.push_back(PosYFilename);
	m_FileNames.push_back(NegYFilename);
	m_FileNames.push_back(PosZFilename);
	m_FileNames.push_back(NegZFilename);

	m_TextureObj = 0;
}

CubemapTexture::~CubemapTexture() {
	DELETE_TEXTURE(m_TextureObj);
}

bool CubemapTexture::Load() {

	glEnable(GL_TEXTURE_CUBE_MAP);

	glGenTextures(1, &m_TextureObj);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureObj);

	for(unsigned int i = 0; i<ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		ImageLoader::LoadImg(m_FileNames[i]);

		glTexImage2D(types[i],
			0,
			ImageLoader::GetImageInternalFormat(),
			ImageLoader::GetImageWidth(),
			ImageLoader::GetImageHeight(),
			0,
			ImageLoader::GetImageFormat(),
			GL_UNSIGNED_BYTE,
			ImageLoader::GetImageData());
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return true;
}

void CubemapTexture::Bind(GLenum textureUnit) const {
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureObj);
}

void CubemapTexture::Unbind(GLenum textureUnit) const {
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

const vector<string>& CubemapTexture::GetTextureNames() const {
	return m_FileNames;
}
