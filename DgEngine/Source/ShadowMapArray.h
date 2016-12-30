////////////////////////////////////////////////////////////////////////////////
// Filename: ShadowMapArray.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <GL/glew.h>
#include <memory>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Shader.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class ShadowMapArray {
	public:
		ShadowMapArray();
		virtual ~ShadowMapArray();

		bool Initialize();
		bool IsInitialized() const;
		virtual void BindShadowMapForWriting() const;
		virtual void BindShadowMapForReading(GLenum) const;
		void ShadowMapPass() const;
		virtual void BindShadowCubeMapForWriting() const;
		virtual void BindShadowCubeMapForReading(GLenum) const;
		void ShadowCubeMapPass() const;
		virtual void Unbind() const;

	public:
		unsigned int		m_ShadowMapSize;
		shared_ptr<Shader>	m_ShadowMapShader;
		GLuint				m_ShadowMapFbo;
		GLuint				m_ShadowMap;
		shared_ptr<Shader>	m_ShadowCubeMapShader;
		GLuint				m_ShadowCubeMapFbo;
		GLuint				m_ShadowCubeMap;
	};
}
