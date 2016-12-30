////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderPostProcessing.h
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
	class ShaderPostProcessing : public Shader {
	public:
		ShaderPostProcessing()
			: Shader("./Shaders/post_processing.vs.glsl",
			"", "", "",
			"./Shaders/post_processing.fs.glsl") {
			m_ShaderName = SHADER_POST_PROCESSING;
		}

		virtual ~ShaderPostProcessing() {}
	};
}
