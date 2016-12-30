////////////////////////////////////////////////////////////////////////////////
// Filename: Scene_Physics.h
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
#include "Transform.h"
#include "Utils.h"
/*Scripts*/
#include "CameraCharacterController.h"
#include "CameraFPSControllerScript.h"
#include "FPSRendererScript.h"
#include "RotationAboutTargetScript.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;

class Scene_Physics : public ApplicationScene {
public:
	Scene_Physics(string nameScene)
		: ApplicationScene(nameScene, "./Resources/XmlConfigurationFiles/TestPhysXGraphScene.xml") {}
	virtual ~Scene_Physics() {}

	virtual void LoadBehaviourScripts() {
		SceneObject::AddScriptToSceneObject("Camera", new CameraCharacterController(0.64f, 32.0f));
		//SceneObject::AddScriptToSceneObject("Camera", new CameraFPSControllerScript());
		SceneObject::AddScriptToSceneObject("Camera", new FPSRendererScript());
	}

};
