////////////////////////////////////////////////////////////////////////////////
// Filename: OculusVR.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <GL/glew.h>
#include <OVR_CAPI.h>
#include <string>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////

////////////////
// NAMESPACES //
////////////////
using namespace std;


namespace dg {
	class OculusVR {
	public:
		OculusVR();
		virtual ~OculusVR();
				
		bool Initialize();
		const bool& IsReady() const;
		bool RenderingVR();

	private:
		bool				m_Ready;
		ovrResult			m_Result;
		ovrSession			m_Session;
		ovrMirrorTexture	m_MirrorTexture;
		ovrGraphicsLuid		m_Luid;
		ovrHmdDesc			m_HmdDesc;
		ovrSizei			m_WindowSize;
		GLuint				m_MirrorFBO;
		long long			m_FrameIndex;
		bool				m_IsVisible;
	};
}
