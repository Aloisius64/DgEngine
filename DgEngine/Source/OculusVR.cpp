////////////////////////////////////////////////////////////////////////////////
// Filename: OculusVR.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////
#ifdef _WIN32
#include <Windows.h>
#endif

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "OculusVR.h"
#include "VRAppUtils.h"
#include "ApplicationScene.h"
#include "Camera.h"
#include "Configuration.h"
#include "Environment.h"
#include "Gizmo.h"
#include "SceneObject.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


#define MatrixCopy(Dest, Src)\
			for(size_t i = 0; i<4; i++)\
				for(size_t j = 0; j<4; j++)\
					Dest[i][j] = Src[j][i];

TextureBuffer*		eyeRenderTexture[2] = {nullptr, nullptr};
DepthBuffer*		eyeDepthBuffer[2] = {nullptr, nullptr};

OculusVR::OculusVR() {
	m_Ready = false;
	m_MirrorTexture = nullptr;
	m_MirrorFBO = 0;
	m_FrameIndex = 0;
	m_IsVisible = true;
}

OculusVR::~OculusVR() {
	if(m_Ready) {

		DELETE_FRAMEBUFFER(m_MirrorFBO);

		if(m_MirrorTexture)
			ovr_DestroyMirrorTexture(m_Session, m_MirrorTexture);

		for(int eye = 0; eye<2; ++eye) {
			delete eyeRenderTexture[eye];
			delete eyeDepthBuffer[eye];
		}

		ovr_Destroy(m_Session);

		ovr_Shutdown();
	}
}

bool OculusVR::Initialize() {
	if(m_Ready)
		return true;

	m_Result = ovr_Initialize(nullptr);
	if(!OVR_SUCCESS(m_Result)) {
		dg::DebugConsoleMessage("Failed to initialize ovr.\n");
		m_Ready = false;
		return false;
	}

	m_Result = ovr_Create(&m_Session, &m_Luid);
	if(!OVR_SUCCESS(m_Result)) {
		dg::DebugConsoleMessage("Failed to create ovr session.\n");
		m_Ready = false;
		return false;
	}

	m_HmdDesc = ovr_GetHmdDesc(m_Session);

	// Setup Window and Graphics.
	m_WindowSize.w = Configuration::Instance()->GetScreenWidth();
	m_WindowSize.h = Configuration::Instance()->GetScreenHeigth();

	// Make eye render buffers
	for(int eye = 0; eye<2; ++eye) {
		//ovrSizei idealTextureSize = ovr_GetFovTextureSize(m_Session, ovrEyeType(eye), m_HmdDesc.DefaultEyeFov[eye], 1);
		//eyeRenderTexture[eye] = new TextureBuffer(m_Session, true, true, idealTextureSize, 1, NULL, 1);
		//eyeDepthBuffer[eye] = new DepthBuffer(eyeRenderTexture[eye]->GetSize(), 0);
		eyeRenderTexture[eye] = new TextureBuffer(m_Session, true, true, m_WindowSize, 1, NULL, 1);
		eyeDepthBuffer[eye] = new DepthBuffer(m_WindowSize, 0);

		if(!eyeRenderTexture[eye]->TextureChain) {
			m_Ready = false;
			return false;
		}
	}

	ovrMirrorTextureDesc desc;
	memset(&desc, 0, sizeof(desc));
	desc.Width = m_WindowSize.w;
	desc.Height = m_WindowSize.h;
	desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;

	// Create mirror texture and an FBO used to copy mirror texture to back buffer
	m_Result = ovr_CreateMirrorTextureGL(m_Session, &desc, &m_MirrorTexture);
	if(!OVR_SUCCESS(m_Result)) {
		dg::DebugConsoleMessage("Failed to create mirror texture.\n");
		m_Ready = false;
		return false;
	}

	// Configure the mirror read buffer
	GLuint texId;
	ovr_GetMirrorTextureBufferGL(m_Session, m_MirrorTexture, &texId);

	glGenFramebuffers(1, &m_MirrorFBO);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_MirrorFBO);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);
	glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	// Turn off vsync to let the compositor do its magic
#ifdef _WIN32
	// Turn on vertical screen sync under Windows.
	// (I.e. it uses the WGL_EXT_swap_control extension)
	typedef BOOL(WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int interval);
	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");
	if(wglSwapIntervalEXT)	wglSwapIntervalEXT(0);
#endif

	// FloorLevel will give tracking poses where the floor height is 0
	ovr_SetTrackingOriginType(m_Session, ovrTrackingOrigin_FloorLevel);

	m_Ready = true;

	return true;
}

const bool& OculusVR::IsReady() const {
	return m_Ready;
}

bool OculusVR::RenderingVR() {

	// Call ovr_GetRenderDesc each frame to get the ovrEyeRenderDesc, as the returned values (e.g. HmdToEyeOffset) may change at runtime.
	ovrEyeRenderDesc eyeRenderDesc[2];
	eyeRenderDesc[0] = ovr_GetRenderDesc(m_Session, ovrEye_Left, m_HmdDesc.DefaultEyeFov[0]);
	eyeRenderDesc[1] = ovr_GetRenderDesc(m_Session, ovrEye_Right, m_HmdDesc.DefaultEyeFov[1]);

	// Get eye poses, feeding in correct IPD offset
	ovrPosef EyeRenderPose[2];
	ovrVector3f HmdToEyeOffset[2] = {eyeRenderDesc[0].HmdToEyeOffset, eyeRenderDesc[1].HmdToEyeOffset};

	double sensorSampleTime;    // sensorSampleTime is fed into the layer later
	ovr_GetEyePoses(m_Session, m_FrameIndex, ovrTrue, HmdToEyeOffset, EyeRenderPose, &sensorSampleTime);

	if(m_IsVisible) {
		// Keyboard inputs to adjust player orientation
		//static float Yaw(3.141592f);

		for(int eye = 0; eye<2; ++eye) {
#pragma region ViewProjectionMatricesSetup
			Matrix4f finalRollPitchYaw = Matrix4f(EyeRenderPose[eye].Orientation);

			vec3 cameraUp = Camera::ActiveCamera()->GetUp();
			Vector3f finalUp = finalRollPitchYaw.Transform(Vector3f(cameraUp.x,
				cameraUp.y,
				cameraUp.z));

			vec3 cameraTarget = Camera::ActiveCamera()->GetTarget();
			Vector3f finalForward = finalRollPitchYaw.Transform(Vector3f(cameraTarget.x,
				cameraTarget.y,
				cameraTarget.z));

			Vector3f cameraPosition(Camera::ActiveCamera()->GetPosition().x,
				Camera::ActiveCamera()->GetPosition().y,
				Camera::ActiveCamera()->GetPosition().z);
			Vector3f shiftedEyePos = cameraPosition+EyeRenderPose[eye].Position;

			Matrix4f view = Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos+finalForward, finalUp);
			MatrixCopy(Camera::ActiveCamera()->m_ViewMatrix, view.M);

			Matrix4f proj = ovrMatrix4f_Projection(m_HmdDesc.DefaultEyeFov[eye],
				Camera::ActiveCamera()->GetZNear(),
				Camera::ActiveCamera()->GetZFar(),
				ovrProjection_None);
			MatrixCopy(Camera::ActiveCamera()->m_ProjectionMatrix, proj.M);
#pragma endregion

			// Shadows rendering.
			Environment::Instance()->RenderShadows();

			// Switch to eye render target
			eyeRenderTexture[eye]->SetAndClearRenderSurface(eyeDepthBuffer[eye]);

			DataManager::m_DefaultFramebuffer = eyeRenderTexture[eye]->fboId;

			// Rendering.
			Environment::Instance()->RenderAmbientOcclusion();
			Environment::Instance()->RenderScenePostProcessing_Pass1();
#ifdef _DEBUG
			// Gizmo rendering.
			Gizmo::DrawGrid();
			Gizmo::DrawGizmo(eGizmo::AXIS_GIZMO);
#endif
			Environment::Instance()->RenderSkybox();
			SceneObject::RenderGraphScene();
			Environment::Instance()->RenderSceneBloomPass();
			Environment::Instance()->RenderScenePostProcessing_Pass2();

			// Avoids an error when calling SetAndClearRenderSurface during next iteration.
			// Without this, during the next while loop iteration SetAndClearRenderSurface
			// would bind a framebuffer with an invalid COLOR_ATTACHMENT0 because the texture ID
			// associated with COLOR_ATTACHMENT0 had been unlocked by calling wglDXUnlockObjectsNV.
			eyeRenderTexture[eye]->UnsetRenderSurface();

			// Commit changes to the textures so they get picked up frame
			eyeRenderTexture[eye]->Commit();

			DataManager::m_DefaultFramebuffer = 0;
		}
	}

	// Do distortion rendering, Present and flush/sync
	ovrLayerEyeFov ld;
	ld.Header.Type = ovrLayerType_EyeFov;
	ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.

	for(int eye = 0; eye<2; ++eye) {
		ld.ColorTexture[eye] = eyeRenderTexture[eye]->TextureChain;
		ld.Viewport[eye] = Recti(eyeRenderTexture[eye]->GetSize());
		ld.Fov[eye] = m_HmdDesc.DefaultEyeFov[eye];
		ld.RenderPose[eye] = EyeRenderPose[eye];
		ld.SensorSampleTime = sensorSampleTime;
	}

	ovrLayerHeader* layers = &ld.Header;
	ovrResult m_Result = ovr_SubmitFrame(m_Session, m_FrameIndex, nullptr, &layers, 1);
	// exit the rendering loop if submit returns an error, will retry on ovrError_DisplayLost
	if(!OVR_SUCCESS(m_Result))
		return false;

	m_IsVisible = (m_Result==ovrSuccess);

	ovrSessionStatus sessionStatus;
	ovr_GetSessionStatus(m_Session, &sessionStatus);
	if(sessionStatus.ShouldQuit)
		return false;

	if(sessionStatus.ShouldRecenter)
		ovr_RecenterTrackingOrigin(m_Session);

	// Blit mirror texture to back buffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_MirrorFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	GLint w = m_WindowSize.w;
	GLint h = m_WindowSize.h;
	glBlitFramebuffer(
		0, h, w, 0,
		0, 0, w, h,
		GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	m_FrameIndex++;

	return true;
}

