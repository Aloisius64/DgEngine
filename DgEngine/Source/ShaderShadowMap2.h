////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderShadowMap2.h
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
	class ShaderShadowMap2 : public Shader {
	public:
		ShaderShadowMap2()
			: Shader("./Shaders/Shadow/shadow_map2.vs.glsl",
			"", "", 
			"./Shaders/Shadow/shadow_map2.gs.glsl",
			"./Shaders/Shadow/shadow_map2.fs.glsl") {
			m_ShaderName = SHADER_SHADOW_MAP2;
		}

		virtual ~ShaderShadowMap2() {}

		virtual void SetParameters(SceneObject* sceneObject) {
			SetActive();

			Transform* transform = GetComponentType(sceneObject, Transform);
			SetFloatMatrix4("ModelMatrix", transform->GetWorldMatrix());
		}
	};
}
