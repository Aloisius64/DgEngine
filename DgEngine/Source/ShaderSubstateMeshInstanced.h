////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderSubstateMeshInstanced.h
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
	class ShaderSubstateMeshInstanced : public ShaderBaseMesh {
	public:
		ShaderSubstateMeshInstanced()
			: ShaderBaseMesh("./Shaders/Substates/substate_mesh_instanced.vs.glsl",
			"", "",
			"./Shaders/Substates/substate_mesh_instanced.gs.glsl",
			"./Shaders/Substates/substate_mesh_instanced.fs.glsl") {
			m_ShaderName = SHADER_SUBSTATE_MESH_INSTANCED;
		}

		virtual ~ShaderSubstateMeshInstanced() {}

		virtual void SetParameters(SceneObject* sceneObject) {
			ShaderBaseMesh::SetParameters(sceneObject);

			// Object transform.
			Transform* transform = GetComponentType(sceneObject, Transform);
			SetFloatMatrix4("MMatrix", transform->GetWorldMatrix());
			SetFloatMatrix4("VPMatrix", Camera::ActiveCamera()->GetProjectionMatrix()*Camera::ActiveCamera()->GetViewMatrix());
		}

	};
}
