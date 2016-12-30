////////////////////////////////////////////////////////////////////////////////
// Filename: ApplicationScene.cpp
////////////////////////////////////////////////////////////////////////////////

//////////////
// INCLUDES //
//////////////
#include <string>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "ApplicationScene.h"
#include "ApplicationManager.h"
#include "Camera.h"
#include "Environment.h"
#include "Frustum.h"
#include "Gizmo.h"
#include "InfoBarRenderer.h"
#include "Physics\PhysX.h"
#include "SceneLoader.h"
#include "System.h"
#include "TextRenderer.h"
#include "TimeClock.h"
#include "UI.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


ApplicationScene::ApplicationScene(string nameScene, string xmlConfigurationFile)
	: m_NameScene(nameScene),
	m_XmlConfigurationFile(xmlConfigurationFile) {}

ApplicationScene::~ApplicationScene() {}

const string& ApplicationScene::SceneName() {
	return m_NameScene;
}

bool ApplicationScene::Initialize() {
#ifdef _DEBUG
	// Set grid dimension.
	Gizmo::SetGridSize(50);
#endif
	return true;
}

void ApplicationScene::Release() {

}

void ApplicationScene::LoadBehaviourScripts() {

}

bool ApplicationScene::Frame() {
	/********************/
	/* Update functions */
	/********************/
	PhysX::UpdatePhysics();					// PhysX update.
	SceneObject::UpdateGraphScene();		// Update() call.
	SceneObject::PostUpdateGraphScene();	// PostUpdate() call.

	// Clear buffers.
	if(Environment::Instance()->IsFogActive())
		dg::System::Instance()->ClearBuffers(Environment::Instance()->GetFogColor());
	else
		dg::System::Instance()->ClearBuffers(Camera::ActiveCamera()->GetBackgroundColor());

	/********************/
	/* Render functions */
	/********************/
	// Call all render function pointer.
	if(Environment::Instance()->IsOVRActive()) {
		// Call Oculus rendering.
		Environment::Instance()->GetOculus().RenderingVR();
	} else {
		// Shadows rendering.
		Environment::Instance()->RenderShadows();
		// OcclusionRendering.
		Environment::OcclusionRendering();
		// SSAO rendering.
		Environment::RenderAmbientOcclusion();
		// PostProcessing first pass rendering.
		Environment::RenderScenePostProcessing_Pass1();
		// Gizmo rendering.
		Gizmo::DrawGrid();
		Gizmo::DrawGizmo(eGizmo::AXIS_GIZMO);
		// Skybox rendering.
		Environment::RenderSkybox();
		// Scene rendering.
		SceneObject::RenderGraphScene();
		// Bloom pass.
		Environment::RenderSceneBloomPass();
		// PostProcessing second pass rendering.
		Environment::RenderScenePostProcessing_Pass2();
	}
	
	// InfoBar rendering.
	InfoBarRenderer::Render();

	// Text rendering.
	TextRenderer::Render();

	// UI rendering.
	UI::RenderUI();

	// Swap buffers.
	dg::System::Instance()->SwapBuffers();

	return true;
}
