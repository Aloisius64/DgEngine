////////////////////////////////////////////////////////////////////////////////
// Filename: ShadowMapFBO.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <GL/glew.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "IShadow.h"


namespace dg {
	class ShadowMapFBO : public IShadow {
	public:
		ShadowMapFBO();
		virtual ~ShadowMapFBO();

		virtual bool Initialize();
		virtual void BindForWriting() const;
		virtual void BindForReading(GLenum) const;
		virtual void Unbind() const;
		virtual void DirectionalShadowPass(Light*) const;
		virtual void PointShadowPass(Light*) const;
		virtual void SpotShadowPass(Light*) const;
	};
}
