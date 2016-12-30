////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderOcclusionRender.h
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
	class ShaderOcclusionRender : public Shader {
	public:
		ShaderOcclusionRender()
			: Shader("./Shaders/occlusion_render.vs.glsl",
			"", "", "",
			"./Shaders/occlusion_render.fs.glsl") {
			m_ShaderName = SHADER_OCCLUSION_RENDER;
		}

		virtual ~ShaderOcclusionRender() {}

		virtual void SetParameters(SceneObject* sceneObject) {
			SetActive();

			// Object transform.
			Transform* transform = GetComponentType(sceneObject, Transform);
			SetFloatMatrix4("MVPMatrix", transform->GetWorldViewProjectionMatrix());
		}
	};
}
