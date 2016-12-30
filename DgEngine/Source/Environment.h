////////////////////////////////////////////////////////////////////////////////
// Filename: Environment.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <AntTweakBar.h>
#include <memory>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "BloomFBO.h"
#include "Common.h"
#include "HdrFBO.h"
#include "IOBuffer.h"
#include "OculusVR.h"
#include "OIT.h"
#include "ShadowMapArray.h"
#include "SkyBox.h"

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class Environment {
	public:
		static Environment* Instance();
		static void Release();
		static void RenderSkybox();
		static void RenderAmbientOcclusion();
		static void RenderShadows();
		static void RenderScenePostProcessing_Pass1();
		static void RenderScenePostProcessing_Pass2();
		static void RenderSceneBloomPass();
		static void OcclusionRendering();

	public:
		virtual ~Environment();

		void DrawUI(TwBar* bar);

		// Skybox.
		bool InitializeSkyBox(const string&,
			const string&,
			const string&,
			const string&,
			const string&,
			const string&);
		void SetSkyboxActive(const bool&);
		const bool& IsSkyboxActive() const;
		const unique_ptr<SkyBox>& GetSkybox() const;
		// Ambient occlusion.
		bool InitializeAO(unsigned int);
		void SetAOActive(const bool&);
		const bool& IsAOActive() const;
		unsigned int GetKernelSize() const;
		// Shadow.
		bool InitializeShadows(unsigned int);
		void SetShadowsActive(const bool&);
		const bool& AreShadowsActive() const;
		unsigned int GetShadowMapSize() const;
		ShadowMapArray& GetShadowMapArray();
		// Gamma.
		void SetGamma(const float&);
		const float& GetGamma() const;
		// HDR.
		bool InitializeHdr(const float& = 1.0f);
		void SetHdrActive(const bool&);
		const bool& IsHdrActive() const;
		void SetExposure(const float&);
		const float& GetExposure() const;		
		// Bloom.
		bool InitializeBloom();
		void SetBloomActive(const bool&);
		const bool& IsBloomActive() const;
		void SetBloomIterations(const unsigned int&);
		const unsigned int& GetBloomIterations() const;
		// Blend.
		void SetBlendingActive(const bool&);
		const bool& IsBlendingActive() const;
		void SetSrcBlendFunc(const eBlendFunc&);
		const eBlendFunc& GetSrcBlendFunc() const;
		void SetDstBlendFunc(const eBlendFunc&);
		const eBlendFunc& GetDstBlendFunc() const;
		void SetBlendEquation(const eBlendEquation&);
		const eBlendEquation& GetBlendEquation() const;
		void SetBlendColor(const vec4&);
		const vec4& GetBlendColor() const;
		// OIT.
		void SetOITActive(const bool&);
		const bool& IsOITActive() const;
		bool InitializeOIT();
		// Fog.
		void SetFogActive(const bool&);
		const bool& IsFogActive() const;
		void SetFogType(const eFogType&);
		const eFogType& GetFogType() const;
		void SetFogDensity(const float&);
		const float& GetFogDensity() const;
		void SetFogColor(const vec4&);
		const vec4& GetFogColor() const;
		// OVR.
		const bool& IsOVRAvailable() const;
		void SetOVRActive(const bool&);
		const bool& IsOVRActive() const;
		OculusVR& GetOculus();
		// OcclusionRendering.
		void SetOcclusionRenderingActive(const bool&);
		const bool& IsOcclusionRenderingActive();
		// Frustum Culling.
		void SetFrustumCullingActive(const bool&);
		const bool& IsFrustumCullingActive();
		// Common.
		void InitializeQuad();
		void RenderQuad();

	private:
		Environment();

	private:
		static unique_ptr<Environment>	m_Instance;
		// Skybox.
		bool							m_SkyboxActive;
		unique_ptr<SkyBox>				m_Skybox;
		// AO.
		bool							m_AOActive;
		unsigned int					m_KernelSize;
		shared_ptr<Shader>				m_BlurShader;
		shared_ptr<Shader>				m_GeometryShader;
		shared_ptr<Shader>				m_SsaoShader;
		shared_ptr<Mesh>				m_Quad;
		IOBuffer						m_GeometryBuffer;
		IOBuffer						m_AoBuffer;
		IOBuffer						m_BlurBuffer;
		// Shadow.
		bool							m_ShadowsInitialized;
		bool							m_ShadowsActive;
		unsigned int					m_SizeShadowMap;
		ShadowMapArray					m_ShadowMapArray;
		// Fog.
		bool							m_FogActive;
		eFogType						m_FogType;
		float							m_FogDensity;
		vec4							m_FogColor;
		// Gamma.
		float							m_Gamma;
		// HDR.
		bool							m_HDRActive;
		float							m_Exposure;
		HdrFBO							m_HdrFBO;
		// Bloom.
		bool							m_BloomActive;
		BloomFBO						m_BloomFBO;
		shared_ptr<Shader>				m_BloomShader;
		unsigned int					m_Iterations;
		// Blending.
		bool							m_BlendingActive;
		eBlendFunc						m_SrcBlendFunc;
		eBlendFunc						m_DstBlendFunc;
		eBlendEquation					m_BlendEquation;
		vec4							m_BlendColor;
		// OIT.
		bool							m_OITActive;
		OIT								m_OIT;
		// OVR.
		bool							m_OVRActive;
		OculusVR						m_Oculus;
		// PostProcessing.
		shared_ptr<Shader>				m_PostProcessingShader;
		// OcclusionRendering.
		bool							m_OcclusionRendering;
		shared_ptr<Shader>				m_OcclusionRenderShader;
		// Frustum Culling
		bool							m_FrustumCullingActive;
		// Common.
		GLuint							m_QuadVAO;
		GLuint							m_QuadVBO;
	};
}
