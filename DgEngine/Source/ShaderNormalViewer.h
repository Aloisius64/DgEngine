////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderNormalViewer.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Shader.h"
#include "Mathematics.h"
#include "Transform.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class ShaderNormalViewer : public Shader {
	public:
		ShaderNormalViewer()
			: Shader("./Shaders/normal_viewer.vs.glsl",
			"", "",
			"./Shaders/normal_viewer.gs.glsl",
			"./Shaders/normal_viewer.fs.glsl") {
			m_ShaderName = SHADER_NORMAL_VIEWER;
		}

		virtual ~ShaderNormalViewer() {}

		virtual void SetParameters(SceneObject* sceneObject) {
			SetActive();

			// Object transform.
			Transform* transform = GetComponentType(sceneObject, Transform);
			SetFloatMatrix4("PMatrix", transform->GetProjectionMatrix());
			SetFloatMatrix3("NormalMatrix", glm::mat3(glm::transpose(glm::inverse(transform->GetWorldViewMatrix()))));
			SetFloatMatrix4("MVPMatrix", transform->GetWorldViewProjectionMatrix());
		}
	};
}
