////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderSubstateMeshVolume.h
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
	class ShaderSubstateMeshVolume : public Shader {
	public:
		ShaderSubstateMeshVolume()
			: Shader("./Shaders/Substates/substate_mesh_volume_transform.vs.glsl",
			"", "", "",
			"./Shaders/Substates/substate_mesh_volume.fs.glsl") {
			m_ShaderName = SHADER_SUBSTATE_MESH_VOLUME;
		}

		virtual ~ShaderSubstateMeshVolume() {}
	};
}
