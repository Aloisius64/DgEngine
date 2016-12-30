////////////////////////////////////////////////////////////////////////////////
// Filename: HdrFBO.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <GL/glew.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////


namespace dg {
	class HdrFBO {
	public:
		HdrFBO();
		virtual ~HdrFBO();

		bool Initialize();
		void BindForWriting();
		void BindForReading(GLenum);
		void BindBloomBuffer(GLenum);
		void Unbind();
		bool IsInitialized() const;

	private:
		GLuint	m_Fbo;
		GLuint	m_ColorBuffer;
		GLuint	m_BloomBuffer;
		GLuint	m_RboDepth;
	};
}
