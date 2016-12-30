////////////////////////////////////////////////////////////////////////////////
// Filename: BloomFBO.h
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
	class BloomFBO {
	public:
		BloomFBO();
		virtual ~BloomFBO();

		bool Initialize();
		void BindFBO(int);
		void BindBuffer(GLenum, int);
		void BindForReading(GLenum);
		void Unbind();
		bool IsInitialized() const;

	private:
		GLuint	m_PingpongFBO[2];
		GLuint	m_PingpongColorbuffers[2];
		int		m_LastBindedBuffer;
	};
}
