////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderShadowCubeMap.h
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
	class ShaderShadowCubeMap : public Shader {
	public:
		ShaderShadowCubeMap()
			: Shader("./Shaders/Shadow/shadow_cube_map.vs.glsl",
			"", "", 
			"./Shaders/Shadow/shadow_cube_map.gs.glsl",
			"./Shaders/Shadow/shadow_cube_map.fs.glsl") {
			m_ShaderName = SHADER_SHADOW_CUBE_MAP;
		}

		virtual ~ShaderShadowCubeMap() {}

		virtual void SetParameters(SceneObject* sceneObject) {
			SetActive();
			Transform* transform = GetComponentType(sceneObject, Transform);
			SetFloatMatrix4("MMatrix", transform->GetWorldMatrix());
		}
	};
}
