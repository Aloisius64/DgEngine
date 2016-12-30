////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderGizmo.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Shader.h"
#include "Material.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class ShaderGizmo : public Shader {
	public:
		ShaderGizmo()
			: Shader("./Shaders/gizmo.vs.glsl",
			"", "", "",
			"./Shaders/gizmo.fs.glsl") {
			m_ShaderName = SHADER_GIZMO;
		}

		virtual ~ShaderGizmo() {}

		virtual void BindMaterialParameters(const Material& material) {
			if(material.IsWireframe())
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// Wireframe
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// Shaded

			// Albedo texture.
			if(material.GetTexture(eTextureType::DIFFUSE_TEXTURE)) {
				material.GetTexture(eTextureType::DIFFUSE_TEXTURE)->Bind(COLOR_TEXTURE_UNIT);
				SetInt("AlbedoMap_bool", true);
			} else {
				SetInt("AlbedoMap_bool", false);
			}
		}
	};
}
