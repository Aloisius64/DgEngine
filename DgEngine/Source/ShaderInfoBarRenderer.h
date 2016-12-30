////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderInfoBarRenderer.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Shader.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class ShaderInfoBarRenderer : public Shader {
	public:
		ShaderInfoBarRenderer()
			: Shader("./Shaders/infobar_renderer.vs.glsl",
			"", "", "",
			"./Shaders/infobar_renderer.fs.glsl") {
			m_ShaderName = SHADER_INFOBAR_RENDERER;
		}

		virtual ~ShaderInfoBarRenderer() {}
	};
}
