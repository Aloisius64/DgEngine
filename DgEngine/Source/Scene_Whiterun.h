////////////////////////////////////////////////////////////////////////////////
// Filename: Scene_Whiterun.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "ApplicationScene.h"
#include "Common.h"
#include "Environment.h"
#include "System.h"
#include "Transform.h"
#include "UI.h"
#include "Utils.h"
/*Scripts*/
#include "CameraCharacterController.h"
#include "EnableDisableScript.h"
#include "FPSRendererScript.h"
#include "LookAtScript.h"
#include "MovementScript.h"
#include "RotationScript.h"
#include "RotationAboutTargetScript.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


#define SET_ENABLE_DISABLE_SCRIPT(tag, component, distance)\
	for each (auto object in SceneObject::FindSceneObjectByTag(tag)) {\
		if(object->GetComponent(component))\
			object->AddScript(new EnableDisableScript(object->GetComponent(component), distance));\
		}

class Scene_Whiterun : public ApplicationScene {
public:
	Scene_Whiterun(string nameScene)
		: ApplicationScene(nameScene, "./Resources/XmlConfigurationFiles/WhiterunGraphScene.xml") {}
	virtual ~Scene_Whiterun() {}

	virtual bool Initialize() {
		Environment::Instance()->SetOcclusionRenderingActive(false);
		return ApplicationScene::Initialize();
	}

	virtual void LoadBehaviourScripts() {
		Script* cameraCharacterController = new CameraCharacterController(0.5f, 46.0f);
		SceneObject::AddScriptToSceneObject("Camera", cameraCharacterController);
		Script* cameraFPSController = new CameraFPSControllerScript();
		cameraFPSController->Disable();
		SceneObject::AddScriptToSceneObject("Camera", cameraFPSController);
		SceneObject::AddScriptToSceneObject("Camera", new FPSRendererScript());

		//Physics* physicsComponent = GetComponentType(SceneObject::FindSceneObjectById("Camera"), Physics);
		//physicsComponent->Disable();
		//cameraFPSController->Enable();
		//cameraCharacterController->Disable();
		
		SET_ENABLE_DISABLE_SCRIPT("Brazier", eComponentType::LIGHT, 32.0f);
		SET_ENABLE_DISABLE_SCRIPT("Brazierbowl", eComponentType::LIGHT, 32.0f);
	}

};
