////////////////////////////////////////////////////////////////////////////////
// Filename: Light.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <string>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Component.h"
#include "Common.h"
#include "IShadow.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;

namespace dg {
	class Environment;

	class Light : public Component {
		friend class Environment;

	public:
		static inline eComponentType ComponentType() {
			return eComponentType::LIGHT;
		}

	public:
		static eLightType ConvertToTypeLight(const string&); 
		static eShadowType ConvertToShadowLight(const string&);
		static void InitializeDefaultDirectionalLight();

	public:
		Light(SceneObject*,
			eLightType = eLightType::DIRECTIONAL_LIGHT,
			vec4 = color_black,
			float = 1.0f,
			float = 1.0f,
			float = 1.0f,
			float = 1.0f,
			float = 0.1f,
			float = 0.0001f,
			float = 30.0f,
			eShadowType = eShadowType::HARD_SHADOW);
		virtual ~Light();

		virtual void Render(const shared_ptr<Shader>& = nullptr, bool = false, const GLuint& = NULL_QUERY);
		virtual void DrawUI(TwBar*);

		void ChangeType(eLightType);

		const enum eLightType& LightType();
		void SetColor(float, float, float, float = 1.0f);
		void SetColor(const vec4);
		const vec4& GetColor() const;
		void SetAmbientIntensity(float);
		const float& GetAmbientIntensity() const;
		void SetDiffuseIntensity(float);
		const float& GetDiffuseIntensity() const;
		void SetSpecularStrength(float);
		const float& GetSpecularStrength() const;
		void SetDirection(const vec3&);
		const vec3 GetDirection() const;
		const vec3 GetPosition() const;
		const quat GetRotation() const;
		void SetConstantAttenuation(float);
		const float& GetConstantAttenuation() const;
		void SetLinearAttenuation(float);
		const float& GetLinearAttenuation() const;
		void SetExponentialAttenuation(float);
		const float& GetExponentialAttenuation() const;
		void SetCutoff(float);
		const float& GetCutoff() const;
		// Shadows.
		void SetShadowType(eShadowType);
		eShadowType	GetShadowType() const;
		//const IShadow* GetShadowFBO() const;
		void SetLightSpaceMatrix(const mat4&);
		const mat4& GetLightSpaceMatrix() const;

	//private:
	//	bool InitializeShadowBuffer();

	private:
		eLightType	m_LightType;
		vec4		m_Color;
		float		m_AmbientIntensity;
		float		m_DiffuseIntensity;
		float		m_SpecularStrength;
		float		m_ConstantAttenuation;
		float		m_LinearAttenuation;
		float		m_ExponentialAttenuation;
		float		m_Cutoff;
		eShadowType	m_ShadowType;
		//IShadow*	m_ShadowFBO;
		mat4		m_LightSpaceMatrix;
	};
}
