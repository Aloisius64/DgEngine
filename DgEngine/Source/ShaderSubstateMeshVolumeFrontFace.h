////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderSubstateMeshVolumeFrontFace.h
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
	class ShaderSubstateMeshVolumeFrontFace : public Shader {
	public:
		ShaderSubstateMeshVolumeFrontFace()
			: Shader("./Shaders/Substates/substate_mesh_volume_transform.vs.glsl",
			"", "", "",
			"./Shaders/Substates/substate_mesh_volume_front_face.fs.glsl") {
			m_ShaderName = SHADER_SUBSTATE_MESH_FRONTFACE;
		}

		virtual ~ShaderSubstateMeshVolumeFrontFace() {}
	};
}
