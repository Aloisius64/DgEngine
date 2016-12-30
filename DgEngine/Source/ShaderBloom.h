////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderBloom.h
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
	class ShaderBloom : public Shader {
	public:
		ShaderBloom()
			: Shader("./Shaders/bloom.vs.glsl",
			"", "", "",
			"./Shaders/bloom.fs.glsl") {
			m_ShaderName = SHADER_BLOOM;
		}

		virtual ~ShaderBloom() {}
	};
}