////////////////////////////////////////////////////////////////////////////////
// Filename: ShadowCubeMapFBO.h
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
	class ShadowCubeMapFBO : public IShadow {
	public:
		ShadowCubeMapFBO();
		virtual ~ShadowCubeMapFBO();

		virtual bool Initialize();
		virtual void BindForWriting() const;
		virtual void BindForReading(GLenum) const;
		virtual void Unbind() const;
		virtual void DirectionalShadowPass(Light*) const;
		virtual void PointShadowPass(Light*) const;
		virtual void SpotShadowPass(Light*) const;
	};
}
