////////////////////////////////////////////////////////////////////////////////
// Filename: Light.cpp
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Light.h"
#include "DataManager.h"
#include "Environment.h"
#include "Gizmo.h"
#include "Mesh.h"
#include "SceneObject.h"
#include "ShadowMapFBO.h"
#include "ShadowCubeMapFBO.h"
#include "Transform.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


#pragma region AntTweakBar
static void TW_CALL SetColorCallback(const void *value, void *clientData) {
	static_cast<Light *>(clientData)->SetColor(*static_cast<const vec4 *>(value));
}
static void TW_CALL GetColorCallback(void *value, void *clientData) {
	*static_cast<vec4 *>(value) = static_cast<Light*>(clientData)->GetColor();
}
static void TW_CALL SetAmbientIntensityCallback(const void *value, void *clientData) {
	static_cast<Light *>(clientData)->SetAmbientIntensity(*static_cast<const float *>(value));
}
static void TW_CALL GetAmbientIntensityCallback(void *value, void *clientData) {
	*static_cast<float *>(value) = static_cast<Light*>(clientData)->GetAmbientIntensity();
}
static void TW_CALL SetDiffuseIntensityCallback(const void *value, void *clientData) {
	static_cast<Light *>(clientData)->SetDiffuseIntensity(*static_cast<const float *>(value));
}
static void TW_CALL GetDiffuseIntensityCallback(void *value, void *clientData) {
	*static_cast<float *>(value) = static_cast<Light*>(clientData)->GetDiffuseIntensity();
}
static void TW_CALL SetSpecularStrengthCallback(const void *value, void *clientData) {
	static_cast<Light *>(clientData)->SetSpecularStrength(*static_cast<const float *>(value));
}
static void TW_CALL GetSpecularStrengthCallback(void *value, void *clientData) {
	*static_cast<float *>(value) = static_cast<Light*>(clientData)->GetSpecularStrength();
}
static void TW_CALL SetDirectionCallback(const void *value, void *clientData) {
	static_cast<Light *>(clientData)->SetDirection(*static_cast<const vec3 *>(value));
}
static void TW_CALL GetDirectionCallback(void *value, void *clientData) {
	*static_cast<vec3 *>(value) = static_cast<Light*>(clientData)->GetDirection();
}
static void TW_CALL SetConstantAttenuationCallback(const void *value, void *clientData) {
	static_cast<Light *>(clientData)->SetConstantAttenuation(*static_cast<const float *>(value));
}
static void TW_CALL GetConstantAttenuationCallback(void *value, void *clientData) {
	*static_cast<float *>(value) = static_cast<Light*>(clientData)->GetConstantAttenuation();
}
static void TW_CALL SetLinearAttenuationCallback(const void *value, void *clientData) {
	static_cast<Light *>(clientData)->SetLinearAttenuation(*static_cast<const float *>(value));
}
static void TW_CALL GetLinearAttenuationCallback(void *value, void *clientData) {
	*static_cast<float *>(value) = static_cast<Light*>(clientData)->GetLinearAttenuation();
}
static void TW_CALL SetExponentialAttenuationCallback(const void *value, void *clientData) {
	static_cast<Light *>(clientData)->SetExponentialAttenuation(*static_cast<const float *>(value));
}
static void TW_CALL GetExponentialAttenuationCallback(void *value, void *clientData) {
	*static_cast<float *>(value) = static_cast<Light*>(clientData)->GetExponentialAttenuation();
}
static void TW_CALL SetCutoffCallback(const void *value, void *clientData) {
	static_cast<Light *>(clientData)->SetCutoff(*static_cast<const float *>(value));
}
static void TW_CALL GetCutoffCallback(void *value, void *clientData) {
	*static_cast<float *>(value) = static_cast<Light*>(clientData)->GetCutoff();
}
void TW_CALL ChangeLightTypeCallback(const void *value, void *clientData) {
	static_cast<Light *>(clientData)->ChangeType(*static_cast<const eLightType *>(value));
}
void TW_CALL GetLightTypeCallback(void *value, void *clientData) {
	*static_cast<eLightType *>(value) = static_cast<Light*>(clientData)->LightType();
}
void TW_CALL SetShadowTypeCallback(const void *value, void *clientData) {
	static_cast<Light *>(clientData)->SetShadowType(*static_cast<const eShadowType *>(value));
}
void TW_CALL GetShadowTypeCallback(void *value, void *clientData) {
	*static_cast<eShadowType *>(value) = static_cast<Light*>(clientData)->GetShadowType();
}
#pragma endregion

eLightType Light::ConvertToTypeLight(const string& lightType) {
	if(lightType=="Point") {
		return eLightType::POINT_LIGHT;
	} else if(lightType=="Spot") {
		return eLightType::SPOT_LIGHT;
	}
	return eLightType::DIRECTIONAL_LIGHT;
}

eShadowType Light::ConvertToShadowLight(const string& shadowType) {
	if(shadowType=="HARD_SHADOW") {
		return eShadowType::HARD_SHADOW;
	} else if(shadowType=="SOFT_SHADOW") {
		return eShadowType::SOFT_SHADOW;
	}
	return eShadowType::NO_SHADOW;
}

void Light::InitializeDefaultDirectionalLight() {
	SceneObject* sceneObject = new SceneObject("MainLight", "Sun");
	Transform* transform = new Transform(sceneObject);
	transform->Rotate(vec_right*180.0f);
	Light* light = new Light(sceneObject);
	light->SetAmbientIntensity(0.1f);
	light->SetDiffuseIntensity(0.8f);
	light->SetSpecularStrength(0.0f);
	light->SetColor(color_white);
	light->SetConstantAttenuation(1.0f);
	light->SetLinearAttenuation(0.1f);
	light->SetExponentialAttenuation(0.0001f);

	SceneObject::Root()->AddChild(sceneObject);
}

Light::Light(SceneObject* sceneObject,
	eLightType lightType,
	vec4 color,
	float ambientIntensity,
	float diffuseIntensity,
	float specularStrength,
	float constantAttenuation,
	float linearAttenuation,
	float exponentialAttenuation,
	float cutoff,
	eShadowType shadowType)
	: Component(eComponentType::LIGHT, sceneObject),
	m_LightType(lightType),
	m_Color(color),
	m_AmbientIntensity(ambientIntensity),
	m_DiffuseIntensity(diffuseIntensity),
	m_SpecularStrength(specularStrength),
	m_ConstantAttenuation(constantAttenuation),
	m_LinearAttenuation(linearAttenuation),
	m_ExponentialAttenuation(exponentialAttenuation),
	m_Cutoff(cutoff),
	m_ShadowType(shadowType) {

	DataManager::AddLight(GetSceneObject()->Id(), this);
}

Light::~Light() {
	DataManager::RemoveLight(GetSceneObject()->Id());
	//FREE_PTR(m_ShadowFBO);
}

void Light::Render(const shared_ptr<Shader>& shader, bool instanced, const GLuint& query) {
#ifdef _DEBUG
	if(shader!=nullptr)
		return;

	Transform* transform = GetComponentType(GetSceneObject(), Transform);
	if(transform==nullptr)
		return;

	if(m_LightType==eLightType::DIRECTIONAL_LIGHT) {
		Gizmo::DrawGizmo(eGizmo::DIRECTIONAL_LIGHT_GIZMO, transform->GetPosition(), -transform->GetRotation());
	} else if(m_LightType==eLightType::POINT_LIGHT) {
		Gizmo::DrawGizmo(eGizmo::POINT_LIGHT_GIZMO, transform->GetPosition(), transform->GetRotation());
	} else if(m_LightType==eLightType::SPOT_LIGHT) {
		Gizmo::DrawGizmo(eGizmo::SPOT_LIGHT_GIZMO, transform->GetPosition(), transform->GetRotation());
	}
#endif
}

void Light::DrawUI(TwBar* bar) {
	char parameter[128];
	memset(parameter, 0, sizeof(parameter));

	// Light component values.
	TwAddButton(bar, "Light", NULL, NULL, "");
	TwAddVarCB(bar, "Light Enabled", TW_TYPE_BOOL8, EnabledCallback, IsEnabledCallback, this, "");
	TwAddVarCB(bar, "Type", TW_TYPE_LIGHT, ChangeLightTypeCallback, GetLightTypeCallback, this, "");
	TwAddVarCB(bar, "Color", TW_TYPE_COLOR4F, SetColorCallback, GetColorCallback, this, "");
	TwAddVarCB(bar, "Direction", TW_TYPE_DIR3F, SetDirectionCallback, GetDirectionCallback, this, "");
	TwAddVarCB(bar, "Ambient", TW_TYPE_FLOAT, SetAmbientIntensityCallback, GetAmbientIntensityCallback, this, "");
	SNPRINTF(parameter, sizeof(parameter), "%s/Ambient min=%g max=%g ", GetSceneObject()->Id().c_str(), 0.0f, 1000.0f);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/Ambient step=%g ", GetSceneObject()->Id().c_str(), 0.01f);
	TwDefine(parameter);
	TwAddVarCB(bar, "Diffuse", TW_TYPE_FLOAT, SetDiffuseIntensityCallback, GetDiffuseIntensityCallback, this, "");
	SNPRINTF(parameter, sizeof(parameter), "%s/Diffuse min=%g max=%g ", GetSceneObject()->Id().c_str(), 0.0f, 1000.0f);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/Diffuse step=%g ", GetSceneObject()->Id().c_str(), 0.01f);
	TwDefine(parameter);
	TwAddVarCB(bar, "SpecularStrength", TW_TYPE_FLOAT, SetSpecularStrengthCallback, GetSpecularStrengthCallback, this, "");
	SNPRINTF(parameter, sizeof(parameter), "%s/SpecularStrength min=%g max=%g ", GetSceneObject()->Id().c_str(), 0.0f, 1.0f);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/SpecularStrength step=%g ", GetSceneObject()->Id().c_str(), 0.05f);
	TwDefine(parameter);
	TwAddButton(bar, "Attenuation", NULL, NULL, "");
	TwAddVarCB(bar, "Constant", TW_TYPE_FLOAT, SetConstantAttenuationCallback, GetConstantAttenuationCallback, this, "");
	SNPRINTF(parameter, sizeof(parameter), "%s/Constant min=%d ", GetSceneObject()->Id().c_str(), 0);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/Constant step=%g ", GetSceneObject()->Id().c_str(), 0.005f);
	TwDefine(parameter);
	TwAddVarCB(bar, "Linear", TW_TYPE_FLOAT, SetLinearAttenuationCallback, GetLinearAttenuationCallback, this, "");
	SNPRINTF(parameter, sizeof(parameter), "%s/Linear min=%d ", GetSceneObject()->Id().c_str(), 0);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/Linear step=%g ", GetSceneObject()->Id().c_str(), 0.005f);
	TwDefine(parameter);
	TwAddVarCB(bar, "Exponential", TW_TYPE_FLOAT, SetExponentialAttenuationCallback, GetExponentialAttenuationCallback, this, "");
	SNPRINTF(parameter, sizeof(parameter), "%s/Exponential min=%d ", GetSceneObject()->Id().c_str(), 0);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/Exponential step=%g ", GetSceneObject()->Id().c_str(), 0.001f);
	TwDefine(parameter);
	TwAddVarCB(bar, "Cutoff", TW_TYPE_FLOAT, SetCutoffCallback, GetCutoffCallback, this, "");
	SNPRINTF(parameter, sizeof(parameter), "%s/Cutoff min=%d max=%d ", GetSceneObject()->Id().c_str(), 0, 180);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/Cutoff step=%g ", GetSceneObject()->Id().c_str(), 1.0f);
	TwDefine(parameter);
	TwAddVarCB(bar, "ShadowType", TW_TYPE_SHADOW, SetShadowTypeCallback, GetShadowTypeCallback, this, "");
	TwAddSeparator(bar, "", NULL);
	TwAddSeparator(bar, "", NULL);
}

void Light::ChangeType(eLightType lightType) {
	if(m_LightType==lightType)
		return;

	DataManager::RemoveLight(GetSceneObject()->Id());
	m_LightType = lightType;
	DataManager::AddLight(GetSceneObject()->Id(), this);
	//InitializeShadowBuffer();
}

const eLightType& Light::LightType() {
	return m_LightType;
}

void Light::SetColor(float r, float g, float b, float a) {
	m_Color = vec4(r, g, b, a);
}

void Light::SetColor(const vec4 color) {
	m_Color = color;
}

const vec4& Light::GetColor() const {
	return m_Color;
}

void Light::SetAmbientIntensity(float value) {
	m_AmbientIntensity = value;
}

const float& Light::GetAmbientIntensity() const {
	return m_AmbientIntensity;
}

void Light::SetDiffuseIntensity(float value) {
	m_DiffuseIntensity = value;
}

const float& Light::GetDiffuseIntensity() const {
	return m_DiffuseIntensity;
}

void Light::SetSpecularStrength(float value) {
	m_SpecularStrength = value;
}

const float& Light::GetSpecularStrength() const {
	return m_SpecularStrength;
}

void Light::SetDirection(const vec3& value) {
	Transform* transform = GetComponentType(GetSceneObject(), Transform);
	vec3 target = normalize(value);
	quat rotation = rotationBetweenVectors(vec_forward, target);
	transform->SetRotation(rotation);
}

const vec3 Light::GetDirection() const {
	return dg::DirectionFromRotation(vec_forward, GetRotation());
}

const vec3 Light::GetPosition() const {
	Transform* transform = GetComponentType(m_SceneObject, Transform);
	mat4 worldMatrix = transform->GetWorldMatrix();
	vec3 position(worldMatrix[3][0], worldMatrix[3][1], worldMatrix[3][2]);
	return position;
}

const quat Light::GetRotation() const {
	SceneObject* sceneObject = m_SceneObject;
	Transform* transform = GetComponentType(sceneObject, Transform);
	quat rotation = transform->GetRotation();
	while(sceneObject->Parent()) {
		transform = GetComponentType(sceneObject->Parent(), Transform);
		rotation = rotation * transform->GetRotation();
		sceneObject = sceneObject->Parent();
	}
	return rotation;
}

void Light::SetConstantAttenuation(float value) {
	m_ConstantAttenuation = value;
}

const float& Light::GetConstantAttenuation() const {
	return m_ConstantAttenuation;
}

void Light::SetLinearAttenuation(float value) {
	m_LinearAttenuation = value;
}

const float& Light::GetLinearAttenuation() const {
	return m_LinearAttenuation;
}

void Light::SetExponentialAttenuation(float value) {
	m_ExponentialAttenuation = value;
}

const float& Light::GetExponentialAttenuation() const {
	return m_ExponentialAttenuation;
}

void Light::SetCutoff(float value) {
	m_Cutoff = value;
}

const float& Light::GetCutoff() const {
	return m_Cutoff;
}

void Light::SetShadowType(eShadowType shadowType) {
	m_ShadowType = shadowType;
}

eShadowType	Light::GetShadowType() const {
	return m_ShadowType;
}

//const IShadow* Light::GetShadowFBO() const {
//	return m_ShadowFBO;
//}

void Light::SetLightSpaceMatrix(const mat4& value) {
	m_LightSpaceMatrix = value;
}

const mat4& Light::GetLightSpaceMatrix() const {
	return m_LightSpaceMatrix;
}

//bool Light::InitializeShadowBuffer() {
//	if(!Environment::Instance()->AreShadowsActive())
//		return false;
//
//	bool result = false;
//	FREE_PTR(m_ShadowFBO);
//
//	if(m_LightType==eLightType::DIRECTIONAL_LIGHT||m_LightType==eLightType::SPOT_LIGHT) {
//		m_ShadowFBO = new ShadowMapFBO();
//	} else if(m_LightType==eLightType::POINT_LIGHT) {
//		m_ShadowFBO = new ShadowCubeMapFBO();
//	}
//
//	if(m_ShadowFBO) {
//		result = m_ShadowFBO->Initialize();
//	}
//
//	if(!result) {
//		FREE_PTR(m_ShadowFBO);
//		DebugConsoleMessage("Error to initialize shadow buffer. ");
//		DebugConsoleMessage("Light: ");
//		DebugConsoleMessage(GetSceneObject()->Id().c_str());
//		DebugConsoleMessage("\n");
//	}
//
//	return result;
//}
