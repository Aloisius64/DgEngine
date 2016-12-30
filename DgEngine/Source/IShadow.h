////////////////////////////////////////////////////////////////////////////////
// Filename: IShadow.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <GL/glew.h>
#include <memory>

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class Shader;
	class Light;

	class IShadow {
	public:
		virtual bool Initialize() = 0;
		virtual void BindForWriting() const = 0;
		virtual void BindForReading(GLenum) const = 0;
		virtual void Unbind() const = 0;
		virtual void DirectionalShadowPass(Light*) const = 0;
		virtual void PointShadowPass(Light*) const = 0;
		virtual void SpotShadowPass(Light*) const = 0;

	protected:
		GLuint				m_Fbo;
		GLuint				m_ShadowMap;
		unsigned int		m_SizeShadowMap;
		shared_ptr<Shader>	m_ShadowShader;
	};
}