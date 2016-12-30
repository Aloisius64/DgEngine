////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderShadowCubeMap2.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Shader.h"
#include "Transform.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class ShaderShadowCubeMap2 : public Shader {
	public:
		ShaderShadowCubeMap2()
			: Shader("./Shaders/Shadow/shadow_cube_map2.vs.glsl",
			"", "", 
			"./Shaders/Shadow/shadow_cube_map2.gs.glsl",
			"./Shaders/Shadow/shadow_cube_map2.fs.glsl") {
			m_ShaderName = SHADER_SHADOW_CUBE_MAP2;
		}

		virtual ~ShaderShadowCubeMap2() {}

		virtual void SetParameters(SceneObject* sceneObject) {
			SetActive();

			Transform* transform = GetComponentType(sceneObject, Transform);
			SetFloatMatrix4("MMatrix", transform->GetWorldMatrix());
		}
	};
}
