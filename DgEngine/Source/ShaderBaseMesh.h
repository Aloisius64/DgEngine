////////////////////////////////////////////////////////////////////////////////
// Filename: ShaderBaseMesh.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Shader.h"
#include "Camera.h"
#include "Configuration.h"
#include "Environment.h"
#include "Light.h"
#include "Transform.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;

#define SET_LIGHT_TO_SHADER(light, local, spot, indexLight)\
	SNPRINTF(Name, sizeof(Name), "Lights[%d].color", indexLight);\
	SetFloatVector4(Name, light->GetColor());\
	SNPRINTF(Name, sizeof(Name), "Lights[%d].ambient_intensity", indexLight);\
	SetFloat(Name, light->GetAmbientIntensity());\
	SNPRINTF(Name, sizeof(Name), "Lights[%d].diffuse_intensity", indexLight);\
	SetFloat(Name, light->GetDiffuseIntensity());\
	SNPRINTF(Name, sizeof(Name), "Lights[%d].specular_strength", indexLight);\
	SetFloat(Name, light->GetSpecularStrength());\
	SNPRINTF(Name, sizeof(Name), "Lights[%d].position", indexLight);\
	SetFloatVector3(Name, light->GetPosition());\
	SNPRINTF(Name, sizeof(Name), "Lights[%d].direction", indexLight);\
	SetFloatVector3(Name, light->GetDirection());\
	SNPRINTF(Name, sizeof(Name), "Lights[%d].cutoff", indexLight);\
	SetFloat(Name, cosf(glm::radians(light->GetCutoff())));\
	SNPRINTF(Name, sizeof(Name), "Lights[%d].constant_attenuation", indexLight);\
	SetFloat(Name, light->GetConstantAttenuation());\
	SNPRINTF(Name, sizeof(Name), "Lights[%d].linear_attenuation", indexLight);\
	SetFloat(Name, light->GetLinearAttenuation());\
	SNPRINTF(Name, sizeof(Name), "Lights[%d].exponential_attenuation", indexLight);\
	SetFloat(Name, light->GetExponentialAttenuation());\
	SNPRINTF(Name, sizeof(Name), "Lights[%d].isLocal", indexLight);\
	SetInt(Name, local);\
	SNPRINTF(Name, sizeof(Name), "Lights[%d].isSpot", indexLight);\
	SetInt(Name, spot);\
	SNPRINTF(Name, sizeof(Name), "Lights[%d].shadowType", indexLight); \
	SetInt(Name, light->GetShadowType());

namespace dg {
	class ShaderBaseMesh : public Shader {
	public:
		ShaderBaseMesh()
			: Shader("./Shaders/base_mesh.vs.glsl",
			"", "", "",
			"./Shaders/base_mesh.fs.glsl") {
			m_ShaderName = SHADER_BASE_MESH;
		}
		ShaderBaseMesh(string vertexShader,
			string tessellationControlShader,
			string tessellationEvaluationShader,
			string geometryShader,
			string fragmentShader) :
			Shader(vertexShader,
			tessellationControlShader,
			tessellationEvaluationShader,
			geometryShader,
			fragmentShader) {}

		virtual ~ShaderBaseMesh() {}

		virtual void SetParameters(SceneObject* sceneObject) {
			char Name[128];
			memset(Name, 0, sizeof(Name));
			int numLight = -1;
			int numShadow = 0;

			SetActive();

			// Textures.
			SetInt("AlbedoMap", COLOR_TEXTURE_UNIT_INDEX);
			SetInt("NormalMap", NORMAL_TEXTURE_UNIT_INDEX);
			SetInt("SpecularMap", SPECULAR_TEXTURE_UNIT_INDEX);
			SetInt("ShininessMap", SHININESS_TEXTURE_UNIT_INDEX);
			SetInt("AmbientMap", AMBIENT_TEXTURE_UNIT_INDEX);
			SetInt("EmissiveMap", EMISSIVE_TEXTURE_UNIT_INDEX);
			SetInt("OpacityMap", OPACITY_TEXTURE_UNIT_INDEX);
			SetInt("Skybox", SKYBOX_TEXTURE_UNIT_INDEX);
			SetInt("ShadowMapArray", SHADOW_MAP_ARRAY_TEXTURE_UNIT_INDEX);
			SetInt("ShadowCubeMapArray", SHADOW_CUBE_MAP_ARRAY_TEXTURE_UNIT_INDEX);

			// AO.
			SetInt("AOMap", AO_TEXTURE_UNIT_INDEX);
			SetInt("useAOMap", Environment::Instance()->IsAOActive());
			vec2 screenSize((float) Configuration::Instance()->GetScreenWidth(),
				(float) Configuration::Instance()->GetScreenHeigth());
			SetFloatVector2("ScreenSize", screenSize);

			// Fog.
			SetInt("FogActive", Environment::Instance()->IsFogActive());
			SetInt("FogType", Environment::Instance()->GetFogType());
			SetFloat("FogDensity", Environment::Instance()->GetFogDensity());
			SetFloatVector4("FogColor", Environment::Instance()->GetFogColor());

			// Gamma.
			SetFloat("Gamma", Environment::Instance()->GetGamma());

			// HDR.
			SetInt("ActiveHDR", Environment::Instance()->IsHdrActive());

			// OIT.
			SetInt("ActiveOIT", Environment::Instance()->IsOITActive());

			// Camera position.
			SetFloatVector3("EyePosition", Camera::ActiveCamera()->GetPosition());

			// Far plane.
			SetFloat("FarPlane", Camera::ActiveCamera()->GetZFar());

			// Object transform.
			Transform* transform = GetComponentType(sceneObject, Transform);
			SetFloatMatrix4("MMatrix", transform->GetWorldMatrix());
			SetFloatMatrix3("NormalMatrix", glm::mat3(glm::transpose(glm::inverse(transform->GetWorldViewMatrix()))));
			SetFloatMatrix4("MVPMatrix", transform->GetWorldViewProjectionMatrix());

			// Set point lights.
			for each (auto &light in DataManager::GetPointLightMap()) {
				if(light.second->IsEnabled() && numLight<MAX_SHADOWS) {
					++numLight;
					SET_LIGHT_TO_SHADER(light.second, true, false, numLight);
					// Shadow.
					if(Environment::Instance()->AreShadowsActive()&&
						numLight<MAX_SHADOWS&&
						numShadow==0) {
						SNPRINTF(Name, sizeof(Name), "LightIndex[%d]", numShadow);
						SetInt(Name, numLight);
						SNPRINTF(Name, sizeof(Name), "ShadowType[%d]", numShadow);
						SetInt(Name, light.second->LightType());
						numShadow++;
					}
					SNPRINTF(Name, sizeof(Name), "LightSpaceMatrix[%d]", numLight);
					SetFloatMatrix4(Name, mat4(1.0f));
				}
			}

			// Set directional lights.
			for each (auto &light in DataManager::GetDirectionalLightMap()) {
				if(light.second->IsEnabled()&&numLight<MAX_SHADOWS) {
					++numLight;
					SET_LIGHT_TO_SHADER(light.second, false, false, numLight);
					// Shadow.
					if(Environment::Instance()->AreShadowsActive()&&numLight<MAX_SHADOWS) {
						SNPRINTF(Name, sizeof(Name), "LightIndex[%d]", numShadow);
						SetInt(Name, numLight);
						SNPRINTF(Name, sizeof(Name), "ShadowType[%d]", numShadow);
						SetInt(Name, light.second->LightType());
						SNPRINTF(Name, sizeof(Name), "LightSpaceMatrix[%d]", numLight);
						SetFloatMatrix4(Name, light.second->GetLightSpaceMatrix());
						numShadow++;
					} else {
						SNPRINTF(Name, sizeof(Name), "LightSpaceMatrix[%d]", numLight);
						SetFloatMatrix4(Name, mat4(1.0f));
					}
				}
			}

			// Set spot lights.
			for each (auto &light in DataManager::GetSpotLightMap()) {
				if(light.second->IsEnabled()&&numLight<MAX_SHADOWS) {
					++numLight;
					SET_LIGHT_TO_SHADER(light.second, true, true, numLight);
					// Shadow.
					if(Environment::Instance()->AreShadowsActive()&&numLight<MAX_SHADOWS) {
						SNPRINTF(Name, sizeof(Name), "LightIndex[%d]", numShadow);
						SetInt(Name, numLight);
						SNPRINTF(Name, sizeof(Name), "ShadowType[%d]", numShadow);
						SetInt(Name, light.second->LightType());
						SNPRINTF(Name, sizeof(Name), "LightSpaceMatrix[%d]", numLight);
						SetFloatMatrix4(Name, light.second->GetLightSpaceMatrix());
						numShadow++;
					} else {
						SNPRINTF(Name, sizeof(Name), "LightSpaceMatrix[%d]", numLight);
						SetFloatMatrix4(Name, mat4(1.0f));
					}
				}
			}

			// Num lights.
			if(numLight>=0) {
				SetInt("NumLights", numLight+1);
			} else {
				SetInt("NumLights", 0);
			}

			// Num shadows.
			SetInt("NumShadow", numShadow);
		}

		virtual void BindMaterialParameters(const Material& material) {
			if(material.IsWireframe())
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// Wireframe
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// Shaded

			// Bind material properties.
			SetFloat("material.shininess", material.GetShininess());
			SetFloatVector4("material.ambient", material.GetColorAmbient());
			SetFloatVector4("material.diffuse", material.GetColorDiffuse());
			SetFloatVector4("material.specular", material.GetColorSpecular());
			SetFloatVector4("material.emissive", material.GetColorEmissive());
			SetFloat("material.reflection", material.GetReflectivity());
			SetFloat("material.refraction", material.GetRefraction());
			SetFloat("material.bump_scaling", material.GetBumpScaling());
			SetFloat("material.opacity", material.GetOpacity());
			SetFloatVector2("material.tc_scale", material.GetTcScale());
			SetFloatVector2("material.tc_offset", material.GetTcOffset()/100.0f);

			SetInt("useSkybox", Environment::Instance()->IsSkyboxActive());
			if(Environment::Instance()->IsSkyboxActive()) {
				Environment::Instance()->GetSkybox()->GetCubemapTex()->Bind(SKYBOX_TEXTURE_UNIT);
			}

			// Albedo texture.
			if(material.GetTexture(eTextureType::DIFFUSE_TEXTURE)) {
				material.GetTexture(eTextureType::DIFFUSE_TEXTURE)->Bind(COLOR_TEXTURE_UNIT);
				SetInt("useAlbedoMap", true);
			} else {
				SetInt("useAlbedoMap", false);
			}

			// Normal texture.
			if(material.GetTexture(eTextureType::NORMAL_TEXTURE)) {
				material.GetTexture(eTextureType::NORMAL_TEXTURE)->Bind(NORMAL_TEXTURE_UNIT);
				SetInt("useNormalMap", true);
			} else {
				SetInt("useNormalMap", false);
			}

			// Specular texture.
			if(material.GetTexture(eTextureType::SPECULAR_TEXTURE)) {
				material.GetTexture(eTextureType::SPECULAR_TEXTURE)->Bind(SPECULAR_TEXTURE_UNIT);
				SetInt("useSpecularMap", true);
			} else {
				SetInt("useSpecularMap", false);
			}

			// Shininess texture.
			if(material.GetTexture(eTextureType::SHININESS_TEXTURE)) {
				material.GetTexture(eTextureType::SHININESS_TEXTURE)->Bind(SHININESS_TEXTURE_UNIT);
				SetInt("useShininessMap", true);
			} else {
				SetInt("useShininessMap", false);
			}

			// Ambient texture.
			if(material.GetTexture(eTextureType::AMBIENT_TEXTURE)) {
				material.GetTexture(eTextureType::AMBIENT_TEXTURE)->Bind(AMBIENT_TEXTURE_UNIT);
				SetInt("useAmbientMap", true);
			} else {
				SetInt("useAmbientMap", false);
			}

			// Emissive texture.
			if(material.GetTexture(eTextureType::EMISSIVE_TEXTURE)) {
				material.GetTexture(eTextureType::EMISSIVE_TEXTURE)->Bind(EMISSIVE_TEXTURE_UNIT);
				SetInt("useEmissiveMap", true);
			} else {
				SetInt("useEmissiveMap", false);
			}

			// Opacity texture.
			if(material.GetTexture(eTextureType::OPACITY_TEXTURE)) {
				material.GetTexture(eTextureType::OPACITY_TEXTURE)->Bind(OPACITY_TEXTURE_UNIT);
				SetInt("useOpacityMap", true);
			} else {
				SetInt("useOpacityMap", false);
			}

			// Shadow texture.
			if(Environment::Instance()->AreShadowsActive()) {
				SetInt("shadowEnabled", true);
				Environment::Instance()->GetShadowMapArray().BindShadowMapForReading(SHADOW_MAP_ARRAY_TEXTURE_UNIT);
				Environment::Instance()->GetShadowMapArray().BindShadowCubeMapForReading(SHADOW_CUBE_MAP_ARRAY_TEXTURE_UNIT);
			} else {
				SetInt("shadowEnabled", false);
			}
		}
	};
}
