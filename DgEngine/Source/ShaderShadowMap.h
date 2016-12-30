////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderShadowMap.h
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
	class ShaderShadowMap : public Shader {
	public:
		ShaderShadowMap()
			: Shader("./Shaders/Shadow/shadow_map.vs.glsl",
			"", "", "",
			"./Shaders/Shadow/shadow_map.fs.glsl") {
			m_ShaderName = SHADER_SHADOW_MAP;
		}

		virtual ~ShaderShadowMap() {}

		virtual void SetParameters(SceneObject* sceneObject) {
			SetActive();

			Transform* transform = GetComponentType(sceneObject, Transform);
			SetFloatMatrix4("ModelMatrix", transform->GetWorldMatrix());
		}
	};
}
