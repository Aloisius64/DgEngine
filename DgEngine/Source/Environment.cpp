////////////////////////////////////////////////////////////////////////////////
// Filename: Environment.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////
#include <string>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Environment.h"
#include "Camera.h"
#include "Configuration.h"
#include "DataManager.h"
#include "Gizmo.h"
#include "Glossary.h"
#include "Light.h"
#include "Mesh.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


static const string cubemapNames[6] = {
	"POSITIVE_X",
	"NEGATIVE_X",
	"POSITIVE_Y",
	"NEGATIVE_Y",
	"POSITIVE_Z",
	"NEGATIVE_Z"
};

#pragma region Callbacks
void TW_CALL SetSkyboxActiveCallback(const void *value, void *clientData) {
	static_cast<Environment *>(clientData)->SetSkyboxActive(*static_cast<const bool *>(value));
}
void TW_CALL IsSkyboxActiveCallback(void *value, void *clientData) {
	*static_cast<bool *>(value) = static_cast<Environment*>(clientData)->IsSkyboxActive();
}
void TW_CALL SetAmbientOcclusionActiveCallback(const void *value, void *clientData) {
	if(static_cast<Environment *>(clientData)->GetKernelSize()>0)
		static_cast<Environment *>(clientData)->SetAOActive(*static_cast<const bool *>(value));
}
void TW_CALL IsAmbientOcclusionActiveCallback(void *value, void *clientData) {
	*static_cast<bool *>(value) = static_cast<Environment*>(clientData)->IsAOActive();
}
//void TW_CALL SetKernelSizeCallback(const void *value, void *clientData) {
//	static_cast<Environment *>(clientData)->m_KernelSize = (*static_cast<const unsigned int *>(value));
//}
void TW_CALL GetKernelSizeCallback(void *value, void *clientData) {
	*static_cast<unsigned int *>(value) = static_cast<Environment*>(clientData)->GetKernelSize();
}
void TW_CALL SetShadowActiveCallback(const void *value, void *clientData) {
	static_cast<Environment *>(clientData)->SetShadowsActive(*static_cast<const bool *>(value));
}
void TW_CALL IsShadowActiveCallback(void *value, void *clientData) {
	*static_cast<bool *>(value) = static_cast<Environment*>(clientData)->AreShadowsActive();
}
//void TW_CALL SetShadowMapSizeCallback(const void *value, void *clientData) {
//	static_cast<Environment *>(clientData)->m_SizeShadowMap = (*static_cast<const unsigned int *>(value));
//}
void TW_CALL GetShadowMapSizeCallback(void *value, void *clientData) {
	*static_cast<unsigned int *>(value) = static_cast<Environment*>(clientData)->GetShadowMapSize();
}
void TW_CALL SetActiveFogCallback(const void *value, void *clientData) {
	static_cast<Environment *>(clientData)->SetFogActive(*static_cast<const bool *>(value));
}
void TW_CALL IsFogActiveCallback(void *value, void *clientData) {
	*static_cast<bool *>(value) = static_cast<Environment*>(clientData)->IsFogActive();
}
void TW_CALL SetFogTypeCallback(const void *value, void *clientData) {
	static_cast<Environment *>(clientData)->SetFogType(*static_cast<const eFogType*>(value));
}
void TW_CALL GetFogTypeCallback(void *value, void *clientData) {
	*static_cast<eFogType *>(value) = static_cast<Environment*>(clientData)->GetFogType();
}
void TW_CALL SetFogDensityCallback(const void *value, void *clientData) {
	static_cast<Environment *>(clientData)->SetFogDensity(*static_cast<const float *>(value));
}
void TW_CALL GetFogDensityCallback(void *value, void *clientData) {
	*static_cast<float *>(value) = static_cast<Environment*>(clientData)->GetFogDensity();
}
void TW_CALL SetFogColorCallback(const void *value, void *clientData) {
	static_cast<Environment *>(clientData)->SetFogColor(*static_cast<const vec4 *>(value));
}
void TW_CALL GetFogColorCallback(void *value, void *clientData) {
	*static_cast<vec4 *>(value) = static_cast<Environment*>(clientData)->GetFogColor();
}
void TW_CALL SetGammaCallback(const void *value, void *clientData) {
	static_cast<Environment *>(clientData)->SetGamma(*static_cast<const float *>(value));
}
void TW_CALL GetGammaCallback(void *value, void *clientData) {
	*static_cast<float *>(value) = static_cast<Environment*>(clientData)->GetGamma();
}
void TW_CALL SetHdrActiveCallback(const void *value, void *clientData) {
	static_cast<Environment *>(clientData)->SetHdrActive(*static_cast<const bool *>(value));
}
void TW_CALL IsHdrActiveCallback(void *value, void *clientData) {
	*static_cast<bool *>(value) = static_cast<Environment*>(clientData)->IsHdrActive();
}
void TW_CALL SetExposureCallback(const void *value, void *clientData) {
	static_cast<Environment *>(clientData)->SetExposure(*static_cast<const float *>(value));
}
void TW_CALL GetExposureCallback(void *value, void *clientData) {
	*static_cast<float *>(value) = static_cast<Environment*>(clientData)->GetExposure();
}
void TW_CALL SetBloomActiveCallback(const void *value, void *clientData) {
	static_cast<Environment *>(clientData)->SetBloomActive(*static_cast<const bool *>(value));
}
void TW_CALL IsBloomActiveCallback(void *value, void *clientData) {
	*static_cast<bool *>(value) = static_cast<Environment*>(clientData)->IsBloomActive();
}
void TW_CALL SetBloomIterationsCallback(const void *value, void *clientData) {
	static_cast<Environment *>(clientData)->SetBloomIterations(*static_cast<const unsigned int *>(value));
}
void TW_CALL GetBloomIterationsCallback(void *value, void *clientData) {
	*static_cast<unsigned int *>(value) = static_cast<Environment*>(clientData)->GetBloomIterations();
}
void TW_CALL SetBlendingActiveCallback(const void *value, void *clientData) {
	static_cast<Environment *>(clientData)->SetBlendingActive(*static_cast<const bool *>(value));
}
void TW_CALL IsBlendingActiveCallback(void *value, void *clientData) {
	*static_cast<bool *>(value) = static_cast<Environment*>(clientData)->IsBlendingActive();
}
void TW_CALL SetSrcBlendFuncCallback(const void *value, void *clientData) {
	static_cast<Environment *>(clientData)->SetSrcBlendFunc(*static_cast<const eBlendFunc*>(value));
}
void TW_CALL GetSrcBlendFuncCallback(void *value, void *clientData) {
	*static_cast<eBlendFunc *>(value) = static_cast<Environment*>(clientData)->GetSrcBlendFunc();
}
void TW_CALL SetDstBlendFuncCallback(const void *value, void *clientData) {
	static_cast<Environment *>(clientData)->SetDstBlendFunc(*static_cast<const eBlendFunc*>(value));
}
void TW_CALL GetDstBlendFuncCallback(void *value, void *clientData) {
	*static_cast<eBlendFunc *>(value) = static_cast<Environment*>(clientData)->GetDstBlendFunc();
}
void TW_CALL SetBlendEquationCallback(const void *value, void *clientData) {
	static_cast<Environment *>(clientData)->SetBlendEquation(*static_cast<const eBlendEquation*>(value));
}
void TW_CALL GetBlendEquationCallback(void *value, void *clientData) {
	*static_cast<eBlendEquation *>(value) = static_cast<Environment*>(clientData)->GetBlendEquation();
}
void TW_CALL SetBlendColorCallback(const void *value, void *clientData) {
	static_cast<Environment *>(clientData)->SetBlendColor(*static_cast<const vec4*>(value));
}
void TW_CALL GetBlendColorCallback(void *value, void *clientData) {
	*static_cast<vec4 *>(value) = static_cast<Environment*>(clientData)->GetBlendColor();
}
void TW_CALL SetOITActiveCallback(const void *value, void *clientData) {
	static_cast<Environment *>(clientData)->SetOITActive(*static_cast<const bool *>(value));
}
void TW_CALL IsOITActiveCallback(void *value, void *clientData) {
	*static_cast<bool *>(value) = static_cast<Environment*>(clientData)->IsOITActive();
}
void TW_CALL IsOVRAvailableCallback(void *value, void *clientData) {
	*static_cast<bool *>(value) = static_cast<Environment*>(clientData)->IsOVRAvailable();
}
void TW_CALL SetOVRActiveCallback(const void *value, void *clientData) {
	static_cast<Environment *>(clientData)->SetOVRActive(*static_cast<const bool *>(value));
}
void TW_CALL IsOVRActiveCallback(void *value, void *clientData) {
	*static_cast<bool *>(value) = static_cast<Environment*>(clientData)->IsOVRActive();
}
void TW_CALL SetOcclusionRenderingActiveCallback(const void *value, void *clientData) {
	static_cast<Environment *>(clientData)->SetOcclusionRenderingActive(*static_cast<const bool *>(value));
}
void TW_CALL IsOcclusionRenderingActiveCallback(void *value, void *clientData) {
	*static_cast<bool *>(value) = static_cast<Environment*>(clientData)->IsOcclusionRenderingActive();
}
#pragma endregion

unique_ptr<Environment> Environment::m_Instance = nullptr;

Environment* Environment::Instance() {
	if(!m_Instance) {
		m_Instance = unique_ptr<Environment>(new Environment());
	}
	return m_Instance.get();
}

void Environment::Release() {
	m_Instance = nullptr;
}

void Environment::RenderSkybox() {
	if(!Instance()->IsSkyboxActive())
		return;

	for each (auto camera in Camera::GetAllCameras()) {
		// Set camera as active camera.
		Camera::SetActiveCamera(camera);
		// Update Viewport.
		camera->Update();
		// Render the skybox.
		Instance()->m_Skybox->Render();
	}
}

void Environment::RenderAmbientOcclusion() {
	if(!Instance()->IsAOActive())
		return;

	// Geometry Pass.
	m_Instance->m_GeometryShader->SetActive();
	m_Instance->m_GeometryBuffer.BindForWriting();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	SceneObject::RenderGraphScene(m_Instance->m_GeometryShader);

	// SSAO Pass
	m_Instance->m_SsaoShader->SetActive();
	m_Instance->m_SsaoShader->SetFloatMatrix4("gProj", Camera::ActiveCamera()->GetProjectionMatrix());
	m_Instance->m_GeometryBuffer.BindForReading(POSITION_TEXTURE_UNIT);
	m_Instance->m_AoBuffer.BindForWriting();
	glClear(GL_COLOR_BUFFER_BIT);
	m_Instance->m_Quad->Render(0);

	// Blur Pass
	m_Instance->m_BlurShader->SetActive();
	m_Instance->m_BlurShader->SetInt("gColorMap", INPUT_TEXTURE_UNIT_INDEX);
	m_Instance->m_AoBuffer.BindForReading(INPUT_TEXTURE_UNIT);
	m_Instance->m_BlurBuffer.BindForWriting();
	glClear(GL_COLOR_BUFFER_BIT);
	m_Instance->m_Quad->Render(0);

	Instance()->m_BlurBuffer.BindForReading(AO_TEXTURE_UNIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Environment::RenderShadows() {
	if(!Instance()->AreShadowsActive())
		return;

	// Point lights pass.
	Instance()->GetShadowMapArray().ShadowCubeMapPass();

	// Directional and Spot lights pass.
	Instance()->GetShadowMapArray().ShadowMapPass();
}

void Environment::RenderScenePostProcessing_Pass1() {
	if(!Instance()->IsHdrActive()&&!Instance()->IsOITActive())
		return;

	if(Instance()->IsHdrActive())
		Instance()->m_HdrFBO.BindForWriting();

	if(Instance()->IsOITActive()) {
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		Instance()->m_OIT.ClearBuffers();
	}

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

void Environment::RenderScenePostProcessing_Pass2() {
	if(!Instance()->IsHdrActive()&&!Instance()->IsOITActive())
		return;

	if(Instance()->IsHdrActive())
		Instance()->m_HdrFBO.Unbind();

	// 2. Now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range.
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	Instance()->m_PostProcessingShader->SetActive();
	Instance()->m_PostProcessingShader->SetInt("hdrBuffer", COLOR_TEXTURE_UNIT_INDEX);
	Instance()->m_PostProcessingShader->SetInt("bloomBuffer", BLOOM_TEXTURE_UNIT_INDEX);
	Instance()->m_PostProcessingShader->SetFloat("gamma", Instance()->GetGamma());
	Instance()->m_PostProcessingShader->SetFloat("exposure", Instance()->GetExposure());
	Instance()->m_PostProcessingShader->SetInt("ActiveHDR", Instance()->IsHdrActive());
	Instance()->m_PostProcessingShader->SetInt("ActiveOIT", Instance()->IsOITActive());
	Instance()->m_PostProcessingShader->SetInt("ActiveBloom", Instance()->IsBloomActive());

	if(Instance()->IsHdrActive()) {
		Instance()->m_HdrFBO.BindForReading(COLOR_TEXTURE_UNIT);
		if(Instance()->IsBloomActive()) {
			Instance()->m_BloomFBO.BindForReading(BLOOM_TEXTURE_UNIT);
		}
	}

	Instance()->RenderQuad();

	if(Instance()->IsOITActive()) {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
	}

	dg::CheckGLError();
}

void Environment::RenderSceneBloomPass() {
	if(!Instance()->IsHdrActive())
		return;

	if(!Instance()->IsBloomActive())
		return;

	Instance()->m_BloomShader->SetActive();
	Instance()->m_BloomShader->SetInt("image", BLOOM_TEXTURE_UNIT_INDEX);
	for(unsigned int i = 0; i<Instance()->m_Iterations; i++) {
		Instance()->m_BloomFBO.BindFBO((i+1)%2);
		Instance()->m_BloomShader->SetInt("horizontal", (i+1)%2);
		if(i==0) {
			Instance()->m_HdrFBO.BindBloomBuffer(BLOOM_TEXTURE_UNIT);
		} else {
			Instance()->m_BloomFBO.BindBuffer(BLOOM_TEXTURE_UNIT, (i)%2);
		}
		Instance()->RenderQuad();
	}

	Instance()->m_BloomFBO.Unbind();
}

void Environment::OcclusionRendering() {
	if(!Instance()->IsOcclusionRenderingActive())
		return;

	Instance()->m_OcclusionRenderShader->SetActive();

	SceneObject::OcclusionRenderGraphScene(Instance()->m_OcclusionRenderShader);
}

Environment::Environment() {
	// Skybox.
	m_Skybox = nullptr;
	m_SkyboxActive = false;
	// SSAO.
	m_KernelSize = 0;
	m_AOActive = false;
	// Shadow.
	m_SizeShadowMap = 1024;
	m_ShadowsActive = false;
	m_ShadowsInitialized = false;
	// Fog.
	m_FogActive = false;
	m_FogType = eFogType::LINEAR_FOG;
	m_FogDensity = 1.0f;
	SetFogColor(color_white*0.1f);
	// Gamma.
	m_Gamma = 1.2f;
	// HDR.
	m_HDRActive = false;
	m_Exposure = 1.0f;
	// Bloom.
	m_BloomActive = false;
	m_Iterations = 10;
	// Blending.
	m_BlendingActive = glIsEnabled(GL_BLEND)==GL_TRUE;
	m_SrcBlendFunc = eBlendFunc::BF_SRC_ALPHA;
	m_DstBlendFunc = eBlendFunc::BF_ONE_MINUS_SRC_ALPHA;
	m_BlendEquation = eBlendEquation::BE_FUNC_ADD;
	m_BlendColor = color_black;
	// OIT.
	m_OITActive = false;
	// Quad.
	m_QuadVAO = 0;
	m_Quad = DataManager::Instance()->GetMesh(QUAD_MESH_PATH);
	// OVR.
	m_Oculus.Initialize();
	m_OVRActive = false;
	// Shaders.
	m_BlurShader = nullptr;
	m_GeometryShader = nullptr;
	m_SsaoShader = nullptr;
	m_BloomShader = nullptr;
	m_PostProcessingShader = nullptr;
	// OcclusionRendering.
	m_OcclusionRenderShader = nullptr;
	// Frustum Culling
	m_FrustumCullingActive = true;
	//m_OcclusionRendering = false;
	SetOcclusionRenderingActive(true);
}

Environment::~Environment() {
	m_Skybox = nullptr;
	m_Quad = nullptr;

	// Shaders.
	//m_BlurShader = nullptr;
	//m_GeometryShader = nullptr;
	//m_SsaoShader = nullptr;
	//m_BloomShader = nullptr;
	//m_PostProcessingShader = nullptr;
	//m_OcclusionRenderShader = nullptr;
}

void Environment::DrawUI(TwBar* bar) {
	char parameter[128];
	memset(parameter, 0, sizeof(parameter));

	// OVR.
	TwAddButton(bar, "VR", NULL, NULL, "");
	TwAddVarCB(bar, "VR_Ready", TW_TYPE_BOOL8, NULL, IsOVRAvailableCallback, this, " label='Ready' ");
	TwAddVarCB(bar, "VR_Active", TW_TYPE_BOOL8, SetOVRActiveCallback, IsOVRActiveCallback, this, " label='Active' ");
	TwAddSeparator(bar, "", NULL);

	// Frustum Culling.
	TwAddButton(bar, "FrustumCulling", NULL, NULL, "");
	TwAddVarRW(bar, "FrustumCulling_Active", TW_TYPE_BOOL8, &m_FrustumCullingActive, " label='Active' ");
	TwAddSeparator(bar, "", NULL);

	// OcclusionRendering.
	TwAddButton(bar, "OcclusionRendering", NULL, NULL, "");
	TwAddVarCB(bar, "OcclusionRendering_Active", TW_TYPE_BOOL8, SetOcclusionRenderingActiveCallback, IsOcclusionRenderingActiveCallback, this, " label='Active' ");
	TwAddSeparator(bar, "", NULL);

	// Skybox.
	TwAddButton(bar, "Skybox", NULL, NULL, "");
	TwAddVarCB(bar, "SkyboxActive", TW_TYPE_BOOL8, SetSkyboxActiveCallback, IsSkyboxActiveCallback, this, " label='Active' ");
	if(Environment::GetSkybox()&&Environment::GetSkybox()->GetCubemapTex())
		for(unsigned int i = 0; i<Environment::GetSkybox()->GetCubemapTex()->GetTextureNames().size(); i++) {
			TwAddVarRO(bar, cubemapNames[i].c_str(), TW_TYPE_STDSTRING, &Environment::GetSkybox()->GetCubemapTex()->GetTextureNames()[i], "");
		}
	TwAddSeparator(bar, "", NULL);

	// Ambient occlusion.
	TwAddButton(bar, "Ambient Occlusion", NULL, NULL, "");
	TwAddVarCB(bar, "AOActive", TW_TYPE_BOOL8, SetAmbientOcclusionActiveCallback, IsAmbientOcclusionActiveCallback, this, " label='Active' ");
	TwAddVarCB(bar, "AOKernel", TW_TYPE_UINT32, NULL, GetKernelSizeCallback, this, " label='Kernel size' ");
	TwAddSeparator(bar, "", NULL);

	// Gamma.
	TwAddVarCB(bar, "Gamma", TW_TYPE_FLOAT, SetGammaCallback, GetGammaCallback, this, "");
	SNPRINTF(parameter, sizeof(parameter), "Environment/Gamma min=%g ", 1.0f);
	TwDefine(parameter);
	SNPRINTF(parameter, sizeof(parameter), "Environment/Gamma step=%g ", 0.1f);
	TwDefine(parameter);
	TwAddSeparator(bar, "", NULL);

	// HDR.
	TwAddButton(bar, "HDR", NULL, NULL, "");
	TwAddVarCB(bar, "HDRActive", TW_TYPE_BOOL8, SetHdrActiveCallback, IsHdrActiveCallback, this, " label='Active' ");
	TwAddVarCB(bar, "Exposure", TW_TYPE_FLOAT, SetExposureCallback, GetExposureCallback, this, "");
	SNPRINTF(parameter, sizeof(parameter), "Environment/Exposure step=%g ", 0.001f);
	TwDefine(parameter);
	TwAddSeparator(bar, "", NULL);

	// Bloom.
	TwAddButton(bar, "Bloom", NULL, NULL, "");
	TwAddVarCB(bar, "BloomActive", TW_TYPE_BOOL8, SetBloomActiveCallback, IsBloomActiveCallback, this, " label='Active' ");
	TwAddVarCB(bar, "BloomIterations", TW_TYPE_UINT32, SetBloomIterationsCallback, GetBloomIterationsCallback, this, " label='Iterations' ");
	TwAddSeparator(bar, "", NULL);

	// Blending.
	TwAddButton(bar, "Blending", NULL, NULL, "");
	TwAddVarCB(bar, "BlendingActive", TW_TYPE_BOOL8, SetBlendingActiveCallback, IsBlendingActiveCallback, this, " label='Active' ");
	TwAddVarCB(bar, "SrcBlendFunc", TW_TYPE_BLEND_FUNC, SetSrcBlendFuncCallback, GetSrcBlendFuncCallback, this, " label='Src' ");
	TwAddVarCB(bar, "DstBlendFunc", TW_TYPE_BLEND_FUNC, SetDstBlendFuncCallback, GetDstBlendFuncCallback, this, " label='Dst' ");
	TwAddVarCB(bar, "BlendEquation", TW_TYPE_BLEND_EQUATION, SetBlendEquationCallback, GetBlendEquationCallback, this, " label='Equation' ");
	TwAddVarCB(bar, "BlendColor", TW_TYPE_COLOR4F, SetBlendColorCallback, GetBlendColorCallback, this, " label='Color' ");
	TwAddSeparator(bar, "", NULL);

	// OIT.
	TwAddButton(bar, "OIT", NULL, NULL, "");
	TwAddVarCB(bar, "OITActive", TW_TYPE_BOOL8, SetOITActiveCallback, IsOITActiveCallback, this, " label='Active' ");
	TwAddSeparator(bar, "", NULL);

	// Fog.
	TwAddButton(bar, "Fog", NULL, NULL, "");
	TwAddVarCB(bar, "FogActive", TW_TYPE_BOOL8, SetActiveFogCallback, IsFogActiveCallback, this, " label='Active' ");
	TwAddVarCB(bar, "FogType", TW_TYPE_FOG, SetFogTypeCallback, GetFogTypeCallback, this, " label='Type' ");
	TwAddVarCB(bar, "FogDensity", TW_TYPE_FLOAT, SetFogDensityCallback, GetFogDensityCallback, this, " label='Density' ");
	TwAddVarCB(bar, "FogColor", TW_TYPE_COLOR4F, SetFogColorCallback, GetFogColorCallback, this, " label='Color' ");
	TwAddSeparator(bar, "", NULL);

	// Shadows.
	TwAddButton(bar, "Shadows", NULL, NULL, "");
	TwAddVarCB(bar, "ShadowsActive", TW_TYPE_BOOL8, SetShadowActiveCallback, IsShadowActiveCallback, this, " label='Active' ");
	TwAddVarCB(bar, "Map size", TW_TYPE_UINT32, NULL, GetShadowMapSizeCallback, this, "");
	TwAddSeparator(bar, "", NULL);

}

bool Environment::InitializeSkyBox(const string& xPositiveFilename,
	const string& xNegativeFilename,
	const string& yPositiveFilename,
	const string& yNegativeFilename,
	const string& zPositiveFilename,
	const string& zNegativeFilename) {

	m_SkyboxActive = true;

	// Release previous Skybox.
	m_Skybox = nullptr;

	m_Skybox = unique_ptr<SkyBox>(new SkyBox());
	if(!m_Skybox->Initialize(xPositiveFilename,
		xNegativeFilename,
		yPositiveFilename,
		yNegativeFilename,
		zPositiveFilename,
		zNegativeFilename)) {
		dg::DebugConsoleMessage("Failed to initialize skybox!");
		m_SkyboxActive = false;
	}

	if(!dg::CheckGLError()) {
		dg::DebugConsoleMessage("Skybox intialization error\n");
		m_SkyboxActive = false;
	}

	return m_SkyboxActive;
}

void Environment::SetSkyboxActive(const bool& value) {
	m_SkyboxActive = value;
}

const bool& Environment::IsSkyboxActive() const {
	return m_SkyboxActive;
}

const unique_ptr<SkyBox>& Environment::GetSkybox() const {
	return m_Skybox;
}

bool Environment::InitializeAO(unsigned int kernelSize) {
	m_KernelSize = kernelSize;

	// Shaders.
	m_BlurShader = shared_ptr<Shader>(DataManager::GetShader(SHADER_BLUR));
	m_GeometryShader = shared_ptr<Shader>(DataManager::GetShader(SHADER_GEOMETRY));
	m_SsaoShader = shared_ptr<Shader>(DataManager::GetShader(SHADER_SSAO));

	// Buffers initialization.
	unsigned int windowWidth = (unsigned int) dg::Configuration::Instance()->GetScreenWidth();
	unsigned int windowHeight = (unsigned int) dg::Configuration::Instance()->GetScreenHeigth();

	if(!m_GeometryBuffer.Initialize(windowWidth, windowHeight, true, GL_RGB32F)) {
		return false;
	}

	if(!m_AoBuffer.Initialize(windowWidth, windowHeight, false, GL_R32F)) {
		return false;
	}

	if(!m_BlurBuffer.Initialize(windowWidth, windowHeight, false, GL_R32F)) {
		return false;
	}

	m_AOActive = true;

	if(!dg::CheckGLError()) {
		dg::DebugConsoleMessage("AO intialization error\n");
		return false;
	}

	return true;
}

void Environment::SetAOActive(const bool& value) {
	m_AOActive = value;
}

const bool& Environment::IsAOActive() const {
	return m_AOActive;
}

unsigned int Environment::GetKernelSize() const {
	return m_KernelSize;
}

bool Environment::InitializeShadows(unsigned int sizeShadowMap) {
	// Check a valid shadow map dimension.
	if(sizeShadowMap!=eShadowMapSize::TINY_SHADOW_MAP&&
		sizeShadowMap!=eShadowMapSize::SMALL_SHADOW_MAP&&
		sizeShadowMap!=eShadowMapSize::MEDIUM_SHADOW_MAP&&
		sizeShadowMap!=eShadowMapSize::BIG_SHADOW_MAP&&
		sizeShadowMap!=eShadowMapSize::HUGE_SHADOW_MAP)
		return false;

	if(m_ShadowsInitialized)
		return false;

	m_SizeShadowMap = sizeShadowMap;

	if(m_ShadowMapArray.Initialize()) {
		m_ShadowsActive = true;
		m_ShadowsInitialized = true;
	}

	return true;
}

void Environment::SetShadowsActive(const bool& value) {
	if(value) {
		if(!m_ShadowsInitialized) {
			m_ShadowsActive = InitializeShadows(m_SizeShadowMap);
		} else {
			m_ShadowsActive = value;
		}
	} else {
		m_ShadowsActive = value;
	}
}

const bool& Environment::AreShadowsActive() const {
	return m_ShadowsActive;
}

unsigned int Environment::GetShadowMapSize() const {
	return m_SizeShadowMap;
}

ShadowMapArray& Environment::GetShadowMapArray() {
	return m_ShadowMapArray;
}

void Environment::SetFogActive(const bool& value) {
	m_FogActive = value;
}

const bool& Environment::IsFogActive() const {
	return m_FogActive;
}

void Environment::SetFogType(const eFogType& value) {
	m_FogType = value;
}

const eFogType& Environment::GetFogType() const {
	return m_FogType;
}

void Environment::SetFogDensity(const float& value) {
	m_FogDensity = value<1.0f ? 1.0f : value;
}

const float& Environment::GetFogDensity() const {
	return m_FogDensity;
}

void Environment::SetFogColor(const vec4& value) {
	m_FogColor = value;
	m_FogColor.a = 1.0f;
}

const vec4& Environment::GetFogColor() const {
	return m_FogColor;
}

void Environment::SetGamma(const float& value) {
	m_Gamma = value<1.0f ? 1.0f : value;
}

const float& Environment::GetGamma() const {
	return m_Gamma;
}

bool Environment::InitializeHdr(const float& exposure) {
	m_HDRActive = m_HdrFBO.Initialize()&&m_PostProcessingShader!=NULL;
	m_PostProcessingShader = shared_ptr<Shader>(DataManager::GetShader(SHADER_POST_PROCESSING));

	InitializeQuad();

	m_HDRActive = dg::CheckGLError();

	return m_HDRActive;
}

void Environment::SetHdrActive(const bool& value) {
	if(value && !m_HdrFBO.IsInitialized()) {
		InitializeHdr();
	}
	m_HDRActive = value;
}

const bool& Environment::IsHdrActive() const {
	return m_HDRActive;
}

void Environment::SetExposure(const float& value) {
	m_Exposure = value;
}

const float& Environment::GetExposure() const {
	return m_Exposure;
}

bool Environment::InitializeBloom() {
	m_BloomActive = m_BloomFBO.Initialize()&&m_PostProcessingShader!=NULL;
	m_BloomShader = shared_ptr<Shader>(DataManager::GetShader(SHADER_BLOOM));

	InitializeQuad();

	m_BloomActive = dg::CheckGLError();

	return m_BloomActive;
}

void Environment::SetBloomActive(const bool& value) {
	if(!m_HDRActive)
		return;
	if(value && !m_BloomFBO.IsInitialized()) {
		InitializeBloom();
	}
	m_BloomActive = value;
}

const bool& Environment::IsBloomActive() const {
	return m_BloomActive;
}

void Environment::SetBloomIterations(const unsigned int& value) {
	m_Iterations = clamp((int) value, 2, 100);
}

const unsigned int& Environment::GetBloomIterations() const {
	return m_Iterations;
}

void Environment::SetBlendingActive(const bool& value) {
	m_BlendingActive = value;
	if(value) {
		glEnable(GL_BLEND);
	} else {
		glDisable(GL_BLEND);
	}
}

const bool& Environment::IsBlendingActive() const {
	return m_BlendingActive;
}

void Environment::SetSrcBlendFunc(const eBlendFunc& value) {
	m_SrcBlendFunc = value;
	glBlendFunc(m_SrcBlendFunc, m_DstBlendFunc);
}

const eBlendFunc& Environment::GetSrcBlendFunc() const {
	return m_SrcBlendFunc;
}

void Environment::SetDstBlendFunc(const eBlendFunc& value) {
	m_DstBlendFunc = value;
	glBlendFunc(m_SrcBlendFunc, m_DstBlendFunc);
}

const eBlendFunc& Environment::GetDstBlendFunc() const {
	return m_DstBlendFunc;
}

void Environment::SetBlendEquation(const eBlendEquation& value) {
	m_BlendEquation = value;
	glBlendEquation(m_BlendEquation);
}

const eBlendEquation& Environment::GetBlendEquation() const {
	return m_BlendEquation;
}

void Environment::SetBlendColor(const vec4& value) {
	m_BlendColor = value;
}

const vec4& Environment::GetBlendColor() const {
	return m_BlendColor;
}

void Environment::SetOITActive(const bool& value) {
	if(value && !m_OIT.IsInitialized()) {
		InitializeOIT();
	}
	m_OITActive = value;
}

const bool& Environment::IsOITActive() const {
	return m_OITActive;
}

bool Environment::InitializeOIT() {
	m_OITActive = m_OIT.Initialize()&&m_PostProcessingShader!=nullptr;

	if(m_PostProcessingShader==nullptr)
		m_PostProcessingShader = shared_ptr<Shader>(DataManager::GetShader(SHADER_POST_PROCESSING));

	InitializeQuad();

	m_OITActive = dg::CheckGLError();

	return m_OITActive;
}

void Environment::InitializeQuad() {
	if(m_QuadVAO==0) {
		GLuint quadVAO = 0;
		GLuint quadVBO;
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*) 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (GLvoid*) (3*sizeof(GLfloat)));

		Instance()->m_QuadVAO = quadVAO;
		Instance()->m_QuadVBO = quadVBO;
	}
}

void Environment::RenderQuad() {
	glBindVertexArray(m_QuadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

const bool& Environment::IsOVRAvailable() const {
	return m_Oculus.IsReady();
}

void Environment::SetOVRActive(const bool& value) {
	if(m_Oculus.IsReady())
		m_OVRActive = value;
}

const bool& Environment::IsOVRActive() const {
	return m_OVRActive;
}

OculusVR& Environment::GetOculus() {
	return m_Oculus;
}

void Environment::SetOcclusionRenderingActive(const bool& value) {
	if(value) {
		if(m_OcclusionRenderShader==nullptr)
			m_OcclusionRenderShader = DataManager::GetShader(SHADER_OCCLUSION_RENDER);
		m_OcclusionRendering = m_OcclusionRenderShader!=nullptr;
	} else {
		m_OcclusionRendering = false;
	}
}

const bool& Environment::IsOcclusionRenderingActive() {
	return m_OcclusionRendering;
}

void Environment::SetFrustumCullingActive(const bool& value) {
		m_FrustumCullingActive = value;
}

const bool& Environment::IsFrustumCullingActive() {
	return m_FrustumCullingActive;
}

