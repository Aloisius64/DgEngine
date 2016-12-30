////////////////////////////////////////////////////////////////////////////////
// Filename: Texture.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <AntTweakBar.h>
#include <GL/glew.h>
#include <memory>
#include <string>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "DataManager.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class Texture {
	public:
		static void DrawUITextures(TwBar*, const TextureMap&);
		static void AddTextureButton(TwBar*, const shared_ptr<Texture>&, eTextureType);

	public:
		Texture(const TextureDesc&);
		virtual ~Texture();

		bool Load();
		void Bind(GLenum);
		void DrawUI(TwBar*);

		const string& GetFilePath() const;
		const string& GetFileName() const;
		const GLenum& GetTextureTarget() const;
		const GLint& GetInternalFormat() const;
		const GLsizei& GetWidth() const;
		const GLsizei& GetHeight() const;
		const GLenum& GetFormat() const;
		const bool& GetMipMap() const;
		void SetWrapS(const GLint&);
		const GLint& GetWrapS() const;
		void SetWrapT(const GLint&);
		const GLint& GetWrapT() const;
		void SetMagFilter(const GLint&);
		const GLint& GetMagFilter() const;
		void SetMinFilter(const GLint&);
		const GLint& GetMinFilter() const;

	private:
		void GenerateMipMap();

	private:
		string	m_FilePath;
		string	m_FileName;
		GLuint	m_TextureObj;
		GLenum	m_TextureTarget;
		GLint	m_InternalFormat;
		GLsizei m_Width;
		GLsizei m_Height;
		GLenum	m_Format;
		bool	m_MipMap;
		GLint	m_WrapS;
		GLint	m_WrapT;
		GLint	m_MagFilter;
		GLint	m_MinFilter;
	};
}
