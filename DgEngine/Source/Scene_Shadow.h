////////////////////////////////////////////////////////////////////////////////
// Filename: Scene_Shadow.h
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
#include "CameraFPSControllerScript.h"
#include "MovementScript.h"
#include "RotationScript.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;

class Scene_Shadow : public ApplicationScene {
public:
	Scene_Shadow(string nameScene)
		: ApplicationScene(nameScene, "./Resources/XmlConfigurationFiles/TestShadowGraphScene.xml") {}
	virtual ~Scene_Shadow() {}

	virtual void LoadBehaviourScripts() {
		SceneObject::AddScriptToSceneObject("Camera", new CameraFPSControllerScript());
		SceneObject::AddScriptToSceneObject("Camera", new FPSRendererScript());
		SceneObject::AddScriptToSceneObject("Model1", new RotationScript(vec3(0.0f, 10.0f, 0.0f)));
		SceneObject::AddScriptToSceneObject("Model2", new RotationScript(vec3(0.0f, -5.0f, 0.0f)));
		//SceneObject::AddScriptToSceneObject("Spot1", new MovementScript(vec3(1.0f, 0.0f, 0.0f)));
		//SceneObject::AddScriptToSceneObject("Spot2", new MovementScript(vec3(-1.0f, 0.0f, 0.0f)));
	}

};
