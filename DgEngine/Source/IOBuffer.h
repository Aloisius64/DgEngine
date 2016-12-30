////////////////////////////////////////////////////////////////////////////////
// Filename: IOBuffer.h
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
	class IOBuffer {
	public:
		IOBuffer();
		virtual ~IOBuffer();

		bool Initialize(unsigned int, unsigned int, bool, GLenum);
		void BindForWriting();
		void BindForReading(GLenum);

	private:
		GLuint	m_Fbo;
		GLuint	m_Texture;
		GLuint	m_Depth;
		GLenum	m_InternalType;
	};
}
