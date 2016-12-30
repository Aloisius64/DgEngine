////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderSkybox.h
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
	class ShaderSkybox : public Shader {
	public:
		ShaderSkybox()
			: Shader("./Shaders/skybox.vs.glsl",
			"", "", "",
			"./Shaders/skybox.fs.glsl") {
			m_ShaderName = SHADER_SKYBOX;
		}

		virtual ~ShaderSkybox() {}
	};
}
