////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderTextRenderer.h
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
	class ShaderTextRenderer : public Shader {
	public:
		ShaderTextRenderer()
			: Shader("./Shaders/text_renderer.vs.glsl",
			"", "", "",
			"./Shaders/text_renderer.fs.glsl") {
			m_ShaderName = SHADER_TEXT_RENDERER;
		}

		virtual ~ShaderTextRenderer() {}
	};
}
