////////////////////////////////////////////////////////////////////////////////
// Filename: Material.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <AntTweakBar.h>
#include <memory>
#include <string>
#include <vector>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Common.h"
#include "DataManager.h"
#include "Mathematics.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class Shader;
	class UI;
	class MeshRenderer;
	class Texture;

	class Material {
		friend class UI;
		friend class MeshRenderer;
	public:
		static void DrawUIMaterials(TwBar*, vector<shared_ptr<Material>>&);
		static void DrawUIMaterials(TwBar*, const MaterialMap&);

	public:
		Material(const MaterialDesc&);
		virtual ~Material();

		void BindMaterial();
		void DrawUI(TwBar*);
		
		void SetShader(const shared_ptr<Shader>&);
		const shared_ptr<Shader>& GetShader() const;

		void SetName(string);
		const string& GetName() const;

		void SetWireframe(bool);
		const bool& IsWireframe() const;
		
		void SetOpacity(float);
		const float& GetOpacity() const;

		void SetBumpScaling(float);
		const float& GetBumpScaling() const;

		void SetShininess(float);
		const float& GetShininess() const;

		void SetShininessStrength(float);
		const float& GetShininessStrength() const;

		void SetReflectivity(float);
		const float& GetReflectivity() const;

		void SetRefraction(float);
		const float& GetRefraction() const;

		void SetColorDiffuse(vec4);
		const vec4& GetColorDiffuse() const;

		void SetColorAmbient(vec4);
		const vec4& GetColorAmbient() const;

		void SetColorSpecular(vec4);
		const vec4& GetColorSpecular() const;

		void SetColorEmissive(vec4);
		const vec4& GetColorEmissive() const;
		
		void SetTexture(eTextureType, const shared_ptr<Texture>&);
		const shared_ptr<Texture>& GetTexture(eTextureType) const;

		void SetTcScale(const vec2&);
		const vec2& GetTcScale() const;
		
		void SetTcOffset(const vec2&);
		const vec2& GetTcOffset() const;

	private:
		void LoadTexture(eTextureType, const TextureDesc&);

	private:
		shared_ptr<Shader>			m_Shader;
		string						m_Name;
		bool						m_Wireframe;
		float						m_Opacity;
		float						m_BumpScaling;
		float						m_Shininess;
		float						m_ShininessStrength;
		float						m_Reflectivity;
		float						m_Refraction;
		vec4						m_ColorDiffuse;
		vec4						m_ColorAmbient;
		vec4						m_ColorSpecular;
		vec4						m_ColorEmissive;
		vector<shared_ptr<Texture>>	m_Textures;
		vec2						m_TcScale;
		vec2						m_TcOffset;
	};
}
