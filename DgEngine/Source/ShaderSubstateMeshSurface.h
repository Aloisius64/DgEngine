////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderSubstateMeshSurface.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "ShaderBaseMesh.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class ShaderSubstateMeshSurface : public ShaderBaseMesh {
	public:
		ShaderSubstateMeshSurface()
			: ShaderBaseMesh("./Shaders/Substates/substate_mesh_surface.vs.glsl",
			"", "", "",
			"./Shaders/Substates/substate_mesh_surface.fs.glsl") {
			m_ShaderName = SHADER_SUBSTATE_MESH_SURFACE;
		}

		virtual ~ShaderSubstateMeshSurface() {}
	};
}
