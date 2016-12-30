////////////////////////////////////////////////////////////////////////////////
// Filename: Texture.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Texture.h"
#include "Common.h"
#include "ImageLoader.h"
#include "UI.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


#pragma region Callbacks
void TW_CALL TextureSelectedCallback(void *p) {
	Texture* texture = static_cast<Texture *>(p);
	UI::Instance()->SetActiveTexture(texture);
}
void TW_CALL SetWrapSCallback(const void *value, void *clientData) {
	static_cast<Texture *>(clientData)->SetWrapS(*static_cast<const eTextureWrapParameter*>(value));
}
void TW_CALL GetWrapSCallback(void *value, void *clientData) {
	*static_cast<eTextureWrapParameter *>(value) = eTextureWrapParameter(static_cast<Texture*>(clientData)->GetWrapS());
}
void TW_CALL SetWrapTCallback(const void *value, void *clientData) {
	static_cast<Texture *>(clientData)->SetWrapT(*static_cast<const eTextureWrapParameter*>(value));
}
void TW_CALL GetWrapTCallback(void *value, void *clientData) {
	*static_cast<eTextureWrapParameter *>(value) = eTextureWrapParameter(static_cast<Texture*>(clientData)->GetWrapT());
}
void TW_CALL SetMagFilterCallback(const void *value, void *clientData) {
	static_cast<Texture *>(clientData)->SetMagFilter(*static_cast<const eTextureFilterParameter*>(value));
}
void TW_CALL GetMagFilterCallback(void *value, void *clientData) {
	*static_cast<eTextureFilterParameter *>(value) = eTextureFilterParameter(static_cast<Texture*>(clientData)->GetMagFilter());
}
void TW_CALL SetMinFilterCallback(const void *value, void *clientData) {
	static_cast<Texture *>(clientData)->SetMinFilter(*static_cast<const eTextureFilterParameter*>(value));
}
void TW_CALL GetMinFilterCallback(void *value, void *clientData) {
	*static_cast<eTextureFilterParameter *>(value) = eTextureFilterParameter(static_cast<Texture*>(clientData)->GetMinFilter());
}
#pragma endregion

void Texture::DrawUITextures(TwBar* bar, const TextureMap& texturesMap) {
	for each (auto texture in texturesMap) {
		AddTextureButton(bar, texture.second, eTextureType::NONE_TEXTURE);
	}
}

void Texture::AddTextureButton(TwBar* bar, const shared_ptr<Texture>& texture, eTextureType type) {
	string textureType = "";
	if(type!=eTextureType::NONE_TEXTURE) {
		textureType += type;
		textureType += " ";
	}
	textureType += texture->GetFileName();
	string label = " label='"+texture->GetFileName()+"'";
	TwAddButton(bar, textureType.c_str(), TextureSelectedCallback, (void *) texture.get(), label.c_str());
}

Texture::Texture(const TextureDesc& textureDesc)
	: m_TextureTarget(textureDesc.textureTarget),
	m_FilePath(textureDesc.filePath),
	m_MipMap(textureDesc.mipMap),
	m_WrapS(textureDesc.wrapS),
	m_WrapT(textureDesc.wrapT),
	m_MagFilter(textureDesc.magFilter),
	m_MinFilter(textureDesc.minFilter) {

	m_TextureObj = 0;

	// Check id use slash "/" or back slash "\"
	int lastPos_BackSlash = m_FilePath.find_last_of("\\")+1;
	int lastPos_Slash = m_FilePath.find_last_of("/")+1;
	if(lastPos_BackSlash>lastPos_Slash)
		m_FileName = m_FilePath.substr(lastPos_BackSlash, m_FilePath.size()-lastPos_BackSlash);
	else
		m_FileName = m_FilePath.substr(lastPos_Slash, m_FilePath.size()-lastPos_Slash);
}

Texture::~Texture() {
	DELETE_TEXTURE(m_TextureObj);
}

bool Texture::Load() {
	if(!ImageLoader::LoadImg(m_FilePath)) {
		return false;
	}

	glGenTextures(1, &m_TextureObj);
	glBindTexture(m_TextureTarget, m_TextureObj);

	m_InternalFormat = GL_SRGB_ALPHA; 
	//m_InternalFormat = ImageLoader::GetImageInternalFormat();
	m_Format = ImageLoader::GetImageFormat();
	m_Width = ImageLoader::GetImageWidth();
	m_Height = ImageLoader::GetImageHeight();

	glTexImage2D(m_TextureTarget,
		0,
		m_InternalFormat,
		m_Width,
		m_Height,
		0,
		m_Format,
		GL_UNSIGNED_BYTE,
		ImageLoader::GetImageData());

	SetWrapS(m_WrapS);
	SetWrapT(m_WrapT);
	SetMagFilter(m_MagFilter);
	SetMinFilter(m_MinFilter);
	if(m_MipMap) {
		GenerateMipMap();
	}

	glBindTexture(m_TextureTarget, 0);

	ImageLoader::FreeCPUMemory();

	return dg::CheckGLError();
}

void Texture::Bind(GLenum TextureUnit) {
	glActiveTexture(TextureUnit);
	glBindTexture(m_TextureTarget, m_TextureObj);
}

void Texture::DrawUI(TwBar* bar) {
	char parameter[128];
	memset(parameter, 0, sizeof(parameter));

	// Bar movable?
	SNPRINTF(parameter, sizeof(parameter), "%s movable=true", m_FileName.c_str());
	TwDefine(parameter);

	TwAddVarRO(bar, "FilePath", TW_TYPE_STDSTRING, &m_FilePath, "");
	TwAddVarRO(bar, "FileName", TW_TYPE_STDSTRING, &m_FileName, "");
	//TwAddVarRO(bar, "Target", TW_TYPE_STDSTRING, &m_FileName, "");
	//GLint	m_InternalFormat;
	//GLenum	m_Format;
	TwAddVarRO(bar, "Width", TW_TYPE_INT32, &m_Width, "");
	TwAddVarRO(bar, "Height", TW_TYPE_INT32, &m_Height, "");
	TwAddVarRO(bar, "MipMap", TW_TYPE_BOOL8, &m_MipMap, "");
	TwAddVarCB(bar, "WrapS", TW_TYPE_WRAP, SetWrapSCallback, GetWrapSCallback, this, "");
	TwAddVarCB(bar, "WrapT", TW_TYPE_WRAP, SetWrapTCallback, GetWrapTCallback, this, "");
	TwAddVarCB(bar, "MagFilter", TW_TYPE_FILTER, SetMagFilterCallback, GetMagFilterCallback, this, "");
	TwAddVarCB(bar, "MinFilter", TW_TYPE_FILTER, SetMinFilterCallback, GetMinFilterCallback, this, "");
}

const string& Texture::GetFilePath() const {
	return m_FilePath;
}

const string& Texture::GetFileName() const {
	return m_FileName;
}

const GLenum& Texture::GetTextureTarget() const {
	return m_TextureTarget;
}

const GLint& Texture::GetInternalFormat() const {
	return m_InternalFormat;
}

const GLsizei& Texture::GetWidth() const {
	return m_Width;
}

const GLsizei& Texture::GetHeight() const {
	return m_Height;
}

const GLenum& Texture::GetFormat() const {
	return m_Format;
}

const bool& Texture::GetMipMap() const {
	return m_MipMap;
}

void Texture::SetWrapS(const GLint& value) {
	m_WrapS = value;
	if(glIsTexture(m_TextureObj)) {
		glBindTexture(m_TextureTarget, m_TextureObj);
		glTexParameteri(m_TextureTarget, GL_TEXTURE_WRAP_S, m_WrapS);
		glBindTexture(m_TextureTarget, 0);
	}
}

const GLint& Texture::GetWrapS() const {
	return m_WrapS;
}

void Texture::SetWrapT(const GLint& value) {
	m_WrapT = value;
	if(glIsTexture(m_TextureObj)) {
		glBindTexture(m_TextureTarget, m_TextureObj);
		glTexParameteri(m_TextureTarget, GL_TEXTURE_WRAP_T, m_WrapT);
		glBindTexture(m_TextureTarget, 0);
	}
}

const GLint& Texture::GetWrapT() const {
	return m_WrapT;
}

void Texture::SetMagFilter(const GLint& value) {
	m_MagFilter = value;
	if(glIsTexture(m_TextureObj)) {
		glBindTexture(m_TextureTarget, m_TextureObj);
		glTexParameteri(m_TextureTarget, GL_TEXTURE_MAG_FILTER, m_MagFilter);
		glBindTexture(m_TextureTarget, 0);
	}
}

const GLint& Texture::GetMagFilter() const {
	return m_MagFilter;
}

void Texture::SetMinFilter(const GLint& value) {
	m_MinFilter = value;
	if(glIsTexture(m_TextureObj)) {
		glBindTexture(m_TextureTarget, m_TextureObj);
		glTexParameteri(m_TextureTarget, GL_TEXTURE_MIN_FILTER, m_MinFilter);
		glBindTexture(m_TextureTarget, 0);
	}
}

const GLint& Texture::GetMinFilter() const {
	return m_MinFilter;
}

void Texture::GenerateMipMap() {
	if(glIsTexture(m_TextureObj)) {
		glBindTexture(m_TextureTarget, m_TextureObj);
		glGenerateMipmap(m_TextureTarget);
		glBindTexture(m_TextureTarget, 0);
	}
}
