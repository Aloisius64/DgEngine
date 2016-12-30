////////////////////////////////////////////////////////////////////////////////
// Filename: Material.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Material.h"
#include "DataManager.h"
#include "Environment.h"
#include "Glossary.h"
#include "IShadow.h"
#include "Light.h"
#include "Shader.h"
#include "Texture.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


#pragma region AntTweakBar
static void TW_CALL SetNameCallback(const void *value, void *clientData) {
	static_cast<Material *>(clientData)->SetName(*static_cast<const string *>(value));
}
static void TW_CALL GetNameCallback(void *value, void *clientData) {
	*static_cast<string *>(value) = static_cast<Material*>(clientData)->GetName();
}
static void TW_CALL SetWireframeCallback(const void *value, void *clientData) {
	static_cast<Material *>(clientData)->SetWireframe(*static_cast<const bool *>(value));
}
static void TW_CALL IsWireframeCallback(void *value, void *clientData) {
	*static_cast<bool *>(value) = static_cast<Material*>(clientData)->IsWireframe();
}
static void TW_CALL SetOpacityCallback(const void *value, void *clientData) {
	static_cast<Material *>(clientData)->SetOpacity(*static_cast<const float *>(value));
}
static void TW_CALL GetOpacityCallback(void *value, void *clientData) {
	*static_cast<float *>(value) = static_cast<Material*>(clientData)->GetOpacity();
}
static void TW_CALL SetBumpScalingCallback(const void *value, void *clientData) {
	static_cast<Material *>(clientData)->SetBumpScaling(*static_cast<const float *>(value));
}
static void TW_CALL GetBumpScalingCallback(void *value, void *clientData) {
	*static_cast<float *>(value) = static_cast<Material*>(clientData)->GetBumpScaling();
}
static void TW_CALL SetShininessCallback(const void *value, void *clientData) {
	static_cast<Material *>(clientData)->SetShininess(*static_cast<const float *>(value));
}
static void TW_CALL GetShininessCallback(void *value, void *clientData) {
	*static_cast<float *>(value) = static_cast<Material*>(clientData)->GetShininess();
}
static void TW_CALL SetShininessStrengthCallback(const void *value, void *clientData) {
	static_cast<Material *>(clientData)->SetShininessStrength(*static_cast<const float *>(value));
}
static void TW_CALL GetShininessStrengthCallback(void *value, void *clientData) {
	*static_cast<float *>(value) = static_cast<Material*>(clientData)->GetShininessStrength();
}
static void TW_CALL SetReflectivityCallback(const void *value, void *clientData) {
	static_cast<Material *>(clientData)->SetReflectivity(*static_cast<const float *>(value));
}
static void TW_CALL GetReflectivityCallback(void *value, void *clientData) {
	*static_cast<float *>(value) = static_cast<Material*>(clientData)->GetReflectivity();
}
static void TW_CALL SetRefractionCallback(const void *value, void *clientData) {
	static_cast<Material *>(clientData)->SetRefraction(*static_cast<const float *>(value));
}
static void TW_CALL GetRefractionCallback(void *value, void *clientData) {
	*static_cast<float *>(value) = static_cast<Material*>(clientData)->GetRefraction();
}
static void TW_CALL SetColorDiffuseCallback(const void *value, void *clientData) {
	static_cast<Material *>(clientData)->SetColorDiffuse(*static_cast<const vec4 *>(value));
}
static void TW_CALL GetColorDiffuseCallback(void *value, void *clientData) {
	*static_cast<vec4 *>(value) = static_cast<Material*>(clientData)->GetColorDiffuse();
}
static void TW_CALL SetColorAmbientCallback(const void *value, void *clientData) {
	static_cast<Material *>(clientData)->SetColorAmbient(*static_cast<const vec4 *>(value));
}
static void TW_CALL GetColorAmbientCallback(void *value, void *clientData) {
	*static_cast<vec4 *>(value) = static_cast<Material*>(clientData)->GetColorAmbient();
}
static void TW_CALL SetColorSpecularCallback(const void *value, void *clientData) {
	static_cast<Material *>(clientData)->SetColorSpecular(*static_cast<const vec4 *>(value));
}
static void TW_CALL GetColorSpecularCallback(void *value, void *clientData) {
	*static_cast<vec4 *>(value) = static_cast<Material*>(clientData)->GetColorSpecular();
}
static void TW_CALL SetColorEmissiveCallback(const void *value, void *clientData) {
	static_cast<Material *>(clientData)->SetColorEmissive(*static_cast<const vec4 *>(value));
}
static void TW_CALL GetColorEmissiveCallback(void *value, void *clientData) {
	*static_cast<vec4 *>(value) = static_cast<Material*>(clientData)->GetColorEmissive();
}
static void TW_CALL SetTcScaleCallback(const void *value, void *clientData) {
	static_cast<Material *>(clientData)->SetTcScale(*static_cast<const vec2 *>(value));
}
static void TW_CALL GetTcScaleCallback(void *value, void *clientData) {
	*static_cast<vec2 *>(value) = static_cast<Material*>(clientData)->GetTcScale();
}
static void TW_CALL SetTcOffsetCallback(const void *value, void *clientData) {
	static_cast<Material *>(clientData)->SetTcOffset(*static_cast<const vec2 *>(value));
}
static void TW_CALL GetTcOffsetCallback(void *value, void *clientData) {
	*static_cast<vec2 *>(value) = static_cast<Material*>(clientData)->GetTcOffset();
}
#pragma endregion

static std::string noTexture = "NONE";
static const char* ShaderFile[eShaderData::NUM_SHADERs] = {"Vertex", "Tessellation Control", "Tessellation Evaluation", "Geometry", "Fragment", "Compute"};

#pragma region Callbacks
void TW_CALL MaterialSelectedCallback(void *p) {
	Material* material = static_cast<Material *>(p);
	UI::Instance()->SetActiveMaterial(material);
}
#pragma endregion

void Material::DrawUIMaterials(TwBar* bar, vector<shared_ptr<Material>>& materials) {
	TwAddButton(bar, "Materials", NULL, NULL, "");
	TwAddSeparator(bar, "", NULL);
	// For all materials add a button to control that material.
	for(unsigned int i = 0; i<materials.size(); i++) {
		TwAddButton(bar, materials[i]->m_Name.c_str(), MaterialSelectedCallback, materials[i].get(), "");
	}
}

void Material::DrawUIMaterials(TwBar* bar, const MaterialMap& materialMap) {
	for each (auto material in materialMap) {
		TwAddButton(bar, material.second->m_Name.c_str(), MaterialSelectedCallback, material.second.get(), "");
	}
}

Material::Material(const MaterialDesc& materialDesc)
	: m_Name(materialDesc.materialName),
	m_Wireframe(materialDesc.wireframe),
	m_Opacity(materialDesc.opacity),
	m_BumpScaling(materialDesc.bumpScaling),
	//m_Shininess(materialDesc.shininess),
	m_ShininessStrength(materialDesc.shininessStrength),
	m_Reflectivity(materialDesc.reflectivity),
	m_Refraction(materialDesc.refraction),
	m_ColorDiffuse(materialDesc.colorDiffuse),
	m_ColorAmbient(materialDesc.colorAmbient),
	m_ColorSpecular(materialDesc.colorSpecular),
	m_ColorEmissive(materialDesc.colorEmissive),
	m_TcScale(materialDesc.tc_scale),
	m_TcOffset(materialDesc.tc_offset){

	SetShininess(materialDesc.shininess);

	// Retrieve shader from the map.
	m_Shader = DataManager::GetShader(materialDesc.shaderName);
	
	// Textures loading.
	m_Textures.resize(eTextureType::NUM_TEXTUREs);
	for(unsigned int i = 0; i<eTextureType::NUM_TEXTUREs; i++) {
		LoadTexture(eTextureType(i), materialDesc.textureDesc[i]);
	}
}

Material::~Material() {
	//m_Shader = nullptr;
	for(unsigned int i = 0; i<eTextureType::NUM_TEXTUREs; i++) {
		// With TextureMap textures are released by DataManager
		m_Textures[i] = nullptr;
	}
}

void Material::LoadTexture(eTextureType typeTexture, const TextureDesc& textureDesc) {
	if(textureDesc.filePath!="") {
		m_Textures[typeTexture] = DataManager::TryGetTexture(textureDesc);
		if(!m_Textures[typeTexture]) {
			static TextureDesc missingTexture;
			missingTexture.filePath = MISSING_TEXTURE;
			m_Textures[typeTexture] = DataManager::TryGetTexture(missingTexture);
		}
	}
}

void Material::BindMaterial() {
	if(!m_Shader)
		return;

	m_Shader->BindMaterialParameters(*this);

	dg::CheckGLError();
}

void Material::DrawUI(TwBar* bar) {
	char parameter[128];
	memset(parameter, 0, sizeof(parameter));

	// Bar movable?
	SNPRINTF(parameter, sizeof(parameter), "%s movable=true", m_Name.c_str());
	TwDefine(parameter);

	// Shaders files.
	TwAddButton(bar, "Shader", NULL, NULL, "");
	for(unsigned int i = 0; i<eShaderData::NUM_SHADERs; i++) {
		if(m_Shader->GetFileName(eShaderData(i))!="") {
			TwAddVarRO(bar, ShaderFile[i], TW_TYPE_STDSTRING, &m_Shader->GetFileName(eShaderData(i)), "");
		}
	}
	TwAddSeparator(bar, "", NULL);
	TwAddVarCB(bar, "Wireframe", TW_TYPE_BOOL8, SetWireframeCallback, IsWireframeCallback, this, "");
	TwAddSeparator(bar, "", NULL);
	// Ambient.
	TwAddVarCB(bar, "Color Ambient", TW_TYPE_COLOR4F, SetColorAmbientCallback, GetColorAmbientCallback, this, "");
	if(GetTexture(eTextureType::AMBIENT_TEXTURE)) {
		Texture::AddTextureButton(bar, GetTexture(eTextureType::AMBIENT_TEXTURE), eTextureType::AMBIENT_TEXTURE);
	} else {
		TwAddVarRO(bar, "Texture Ambient", TW_TYPE_STDSTRING, &noTexture, " label='Texture' ");
	}
	TwAddSeparator(bar, "", NULL);
	// Albedo.
	TwAddVarCB(bar, "Color Diffuse", TW_TYPE_COLOR4F, SetColorDiffuseCallback, GetColorDiffuseCallback, this, "");
	if(GetTexture(eTextureType::DIFFUSE_TEXTURE)) {
		Texture::AddTextureButton(bar, GetTexture(eTextureType::DIFFUSE_TEXTURE), eTextureType::DIFFUSE_TEXTURE);
	} else {
		TwAddVarRO(bar, "Texture Diffuse", TW_TYPE_STDSTRING, &noTexture, " label='Texture' ");
	}
	TwAddSeparator(bar, "", NULL);
	// Specular.
	TwAddVarCB(bar, "Color Specular", TW_TYPE_COLOR4F, SetColorSpecularCallback, GetColorSpecularCallback, this, "");
	TwAddVarCB(bar, "Shininess", TW_TYPE_FLOAT, SetShininessCallback, GetShininessCallback, this, "");
	SNPRINTF(parameter, sizeof(parameter), "%s/Shininess min=%g max=%g ", GetName().c_str(), 1.0f, 256.0f);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/Shininess step=%g ", GetName().c_str(), 0.5f);
	TwDefine(parameter);
	if(GetTexture(eTextureType::SPECULAR_TEXTURE)) {
		Texture::AddTextureButton(bar, GetTexture(eTextureType::SPECULAR_TEXTURE), eTextureType::SPECULAR_TEXTURE);
	} else {
		TwAddVarRO(bar, "Texture Specular", TW_TYPE_STDSTRING, &noTexture, " label='Texture' ");
	}
	TwAddSeparator(bar, "", NULL);
	// Normal.
	TwAddVarCB(bar, "BumpScaling", TW_TYPE_FLOAT, SetBumpScalingCallback, GetBumpScalingCallback, this, "");
	SNPRINTF(parameter, sizeof(parameter), "%s/BumpScaling min=%g max=%g ", GetName().c_str(), 0.0f, 10.0f);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/BumpScaling step=%g ", GetName().c_str(), 0.1f);
	TwDefine(parameter);
	if(GetTexture(eTextureType::NORMAL_TEXTURE)) {
		Texture::AddTextureButton(bar, GetTexture(eTextureType::NORMAL_TEXTURE), eTextureType::NORMAL_TEXTURE);
	} else {
		TwAddVarRO(bar, "Texture Normal", TW_TYPE_STDSTRING, &noTexture, " label='Texture' ");
	}
	TwAddSeparator(bar, "", NULL);
	// Opacity.
	TwAddVarCB(bar, "Opacity", TW_TYPE_FLOAT, SetOpacityCallback, GetOpacityCallback, this, "");
	SNPRINTF(parameter, sizeof(parameter), "%s/Opacity min=%g max=%g ", GetName().c_str(), 0.0f, 1.0f);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/Opacity step=%g ", GetName().c_str(), 0.01f);
	TwDefine(parameter);
	if(GetTexture(eTextureType::OPACITY_TEXTURE)) {
		Texture::AddTextureButton(bar, GetTexture(eTextureType::OPACITY_TEXTURE), eTextureType::OPACITY_TEXTURE);
	} else {
		TwAddVarRO(bar, "Texture Opacity", TW_TYPE_STDSTRING, &noTexture, " label='Texture' ");
	}
	TwAddSeparator(bar, "", NULL);
	// Emissive.
	TwAddVarCB(bar, "Color Emissive", TW_TYPE_COLOR4F, SetColorEmissiveCallback, GetColorEmissiveCallback, this, "");
	if(GetTexture(eTextureType::EMISSIVE_TEXTURE)) {
		Texture::AddTextureButton(bar, GetTexture(eTextureType::EMISSIVE_TEXTURE), eTextureType::EMISSIVE_TEXTURE);
	} else {
		TwAddVarRO(bar, "Texture Emissive", TW_TYPE_STDSTRING, &noTexture, " label='Texture' ");
	}
	TwAddSeparator(bar, "", NULL);
	// Reflectivity.
	TwAddVarCB(bar, "Reflectivity", TW_TYPE_FLOAT, SetReflectivityCallback, GetReflectivityCallback, this, "");
	SNPRINTF(parameter, sizeof(parameter), "%s/Reflectivity min=%g max=%g ", GetName().c_str(), 0.0f, 1.0f);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/Reflectivity step=%g ", GetName().c_str(), 0.01f);
	TwDefine(parameter);
	TwAddSeparator(bar, "", NULL);
	// Refraction.
	TwAddVarCB(bar, "Refraction", TW_TYPE_FLOAT, SetRefractionCallback, GetRefractionCallback, this, "");
	SNPRINTF(parameter, sizeof(parameter), "%s/Refraction min=%g max=%g ", GetName().c_str(), 1.0f, 10.0f);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "%s/Refraction step=%g ", GetName().c_str(), 0.01f);
	TwDefine(parameter);
	TwAddSeparator(bar, "", NULL);
	// TexCoords.
	TwAddButton(bar, "TexCoords", NULL, NULL, "");
	TwAddVarCB(bar, "Scale", TW_TYPE_VEC2, SetTcScaleCallback, GetTcScaleCallback, this, "");
	TwAddVarCB(bar, "Offset", TW_TYPE_VEC2, SetTcOffsetCallback, GetTcOffsetCallback, this, "");
	TwAddSeparator(bar, "", NULL);
}

void Material::SetShader(const shared_ptr<Shader>& shader) {
	m_Shader = shader;
}

const shared_ptr<Shader>& Material::GetShader() const {
	return m_Shader;
}

void Material::SetName(string value) {
	m_Name = value;
}

const string& Material::GetName() const {
	return m_Name;
}

void Material::SetWireframe(bool value) {
	m_Wireframe = value;
}

const bool& Material::IsWireframe() const {
	return m_Wireframe;
}

void Material::SetShininess(float value) {
	m_Shininess = glm::clamp(value, 1.0f, 256.0f);
}

const float& Material::GetShininess() const {
	return m_Shininess;
}

void Material::SetOpacity(float value) {
	m_Opacity = value;
}

const float& Material::GetOpacity() const {
	return m_Opacity;
}

void Material::SetBumpScaling(float value) {
	m_BumpScaling = value;
}

const float& Material::GetBumpScaling() const {
	return m_BumpScaling;
}

void Material::SetShininessStrength(float value) {
	if(value>=0.0f && value<=10.0f)
		m_ShininessStrength = value;
}

const float& Material::GetShininessStrength() const {
	return m_ShininessStrength;
}

void Material::SetReflectivity(float value) {
	m_Reflectivity = value;
}

const float& Material::GetReflectivity() const {
	return m_Reflectivity;
}

void Material::SetRefraction(float value) {
	m_Refraction = value;
}

const float& Material::GetRefraction() const {
	return m_Refraction;
}

void Material::SetColorDiffuse(vec4 value) {
	m_ColorDiffuse = value;
}

const vec4& Material::GetColorDiffuse() const {
	return m_ColorDiffuse;
}

void Material::SetColorAmbient(vec4 value) {
	m_ColorAmbient = value;
}

const vec4& Material::GetColorAmbient() const {
	return m_ColorAmbient;
}

void Material::SetColorSpecular(vec4 value) {
	m_ColorSpecular = value;
}

const vec4& Material::GetColorSpecular() const {
	return m_ColorSpecular;
}

void Material::SetColorEmissive(vec4 value) {
	m_ColorEmissive = value;
}

const vec4& Material::GetColorEmissive() const {
	return m_ColorEmissive;
}

void Material::SetTexture(eTextureType typeTexture, const shared_ptr<Texture>& texture) {
	m_Textures[typeTexture] = texture;
}

const shared_ptr<Texture>& Material::GetTexture(eTextureType typeTexture) const {
	return m_Textures[typeTexture];
}

void Material::SetTcScale(const vec2& value) {
	m_TcScale = value;
}

const vec2& Material::GetTcScale() const {
	return m_TcScale;
}

void Material::SetTcOffset(const vec2& value) {
	m_TcOffset = value;
}

const vec2& Material::GetTcOffset() const {
	return m_TcOffset;
}
