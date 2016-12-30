////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderGeometry.h
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
	class ShaderGeometry : public Shader {
	public:
		ShaderGeometry()
			: Shader("./Shaders/SSAO/geometry.vs.glsl",
			"", "", "",
			"./Shaders/SSAO/geometry.fs.glsl") {
			m_ShaderName = SHADER_GEOMETRY;
		}

		virtual ~ShaderGeometry() {}

		virtual void SetParameters(SceneObject* sceneObject) {
			SetActive();
			Transform* transform = GetComponentType(sceneObject, Transform);
			SetFloatMatrix4("gWVP", transform->GetWorldViewProjectionMatrix());
			SetFloatMatrix4("gWV", transform->GetWorldViewMatrix());
		}
	};
}
