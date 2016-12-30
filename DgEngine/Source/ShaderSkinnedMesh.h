////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderSkinnedMesh.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "ShaderBaseMesh.h"
#include "Animator.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class ShaderSkinnedMesh : public ShaderBaseMesh {
	public:
		ShaderSkinnedMesh()
			: ShaderBaseMesh("./Shaders/skinned_mesh.vs.glsl",
			"", "", "",
			"./Shaders/base_mesh.fs.glsl") {
			m_ShaderName = SHADER_SKINNED_MESH;
		}

		virtual ~ShaderSkinnedMesh() {}

		virtual void SetParameters(SceneObject* sceneObject) {
			SetActive();

			Animator* animator = GetComponentType(sceneObject, Animator);
			if(animator) {
				vector<mat4> transforms;
				animator->BoneTransform(transforms);
				SetFloatMatrix4("Bones", transforms);
			}

			ShaderBaseMesh::SetParameters(sceneObject);
		}
	};
}
