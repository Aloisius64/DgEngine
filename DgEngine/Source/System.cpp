////////////////////////////////////////////////////////////////////////////////
// Filename: Systemclass.h
////////////////////////////////////////////////////////////////////////////////


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "System.h"
#include "System_GLFW.h"
#include "Configuration.h"
#include "Environment.h"
#include "Frustum.h"
#include "Gizmo.h"
#include "ImageLoader.h"
#include "InfoBarRenderer.h"
#include "Input.h"
#include "Physics\PhysX.h"
#include "TextRenderer.h"
#include "TimeClock.h"
#include "Utils.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;

unique_ptr<System> System::m_Instance = nullptr;

System::System() {}

System::~System() {}

void System::Shutdown() {
	// Release the gizmo object.
	Gizmo::Release();

	// Release the image loader.
	ImageLoader::Release();

	// Delete the root scene object (Implicitly, clean the entire graph scene).
	SceneObject::DeleteRoot();

	// Release the environment object.
	Environment::Release();

	// Release the data manager.
	DataManager::Release();

	// Release the frustum object.
	Frustum::Release();

	// Release physx.
	PhysX::Release();

	// Release the application manager.
	ApplicationManager::Release();

	// Release the text renderer.
	TextRenderer::Release();

	// Release the infobar renderer.
	InfoBarRenderer::Release();

	// Release the configuration object.
	Configuration::Release();

	// Release the time.
	TimeClock::Release();

	// Release the input.
	Input::Release();
	
	// Shutdown the window.
	ShutdownWindows();

	m_Instance = nullptr;

	return;
}

void System::SwapBuffers() const {

}

void System::ClearBuffers(glm::vec4 clearColor) const {

}

void System::PollEvents() const {

}

void System::UpdateVieport(int x, int y, int width, int height) const {

}

const unique_ptr<System>& System::Instance(enum eGraphicAPI api) {
	if(!m_Instance) {
		switch(api) {
			//#ifdef WIN32
			//		case dg::D3D11:
			//			m_Instance = new dg::System_Windows();
			//			break;
			//#endif
		case dg::OPEN_GL:
			m_Instance = unique_ptr<System>(new dg::System_GLFW());
			break;
		default:
			m_Instance = unique_ptr<System>(new dg::System_GLFW());
			break;
		}
		m_Instance->m_Api = api;
	}

	return m_Instance;
}

void System::Run() {
	bool endRun = false;
	float startRunningTime = TimeClock::Instance()->RunningTime();
	float lastRunningTime = startRunningTime;

	// Initialize the seed.
	SRANDOM;

	// Initialize text renderer.
	TextRenderer::Instance();

	// Initialize info bar renderer.
	InfoBarRenderer::Instance();

	// Loop until there is a quit message from the window or the user.
	while(!endRun) {
		// Updating FPS and DeltaTime
		TimeClock::Instance()->ComputeFPS();
		TimeClock::Instance()->SetDeltaTime(TimeClock::Instance()->RunningTime()-lastRunningTime);
		lastRunningTime = TimeClock::Instance()->RunningTime();
				
		// Check for user input.
		PollEvents();
				
		// Do the frame processing.
		endRun = !ApplicationManager::Frame();

		endRun = endRun||EndRun();
	}

	return;
}

const eGraphicAPI& System::GetGraphicApi() const {
	return m_Api;
}
