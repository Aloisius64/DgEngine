////////////////////////////////////////////////////////////////////////////////
// Filename: Scene_Loader.h
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
#include "MeshRenderer.h"
#include "System.h"
#include "Transform.h"
#include "Utils.h"
/*Scripts*/
#include "CameraFPSControllerScript.h"
#include "FPSRendererScript.h"
#include "LookAtScript.h"
#include "MovementScript.h"
#include "RotationScript.h"
#include "RotationAboutTargetScript.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


class Scene_Loader : public ApplicationScene {
public:
	Scene_Loader(string nameScene)
		: ApplicationScene(nameScene, "./Resources/XmlConfigurationFiles/TestGraphScene.xml") {}
	virtual ~Scene_Loader() {}

	virtual void LoadBehaviourScripts() {
		SceneObject::AddScriptToSceneObject("Camera", new CameraFPSControllerScript());
		SceneObject::AddScriptToSceneObject("Camera", new FPSRendererScript());
		
		SceneObject::AddScriptToSceneObject("House", new RotationScript(vec3(0.0f, 30.0f, 0.0f)));
		Transform* target = nullptr;
		SceneObject* house = SceneObject::FindSceneObjectById("House");
		if(SceneObject::FindSceneObjectById("House"))
			target = GetComponentType(house, Transform);

		SceneObject::AddScriptToSceneObject("House2", new RotationAboutTargetScript(target, 90.0f));
		SceneObject::AddScriptToSceneObject("House2", new RotationScript(vec3(0.0f, -90.0f, 0.0f)));

		SceneObject::AddScriptToSceneObject("Stone", new MovementScript(vec3(1.0f, 0.0f, 0.0f)));
		if(house) {
			Transform* transform = GetComponentType(house, Transform);
			SceneObject::AddScriptToSceneObject("Stone", new LookAtScript(transform));
		}

		SceneObject* giant = SceneObject::FindSceneObjectById("Character1");
		if(giant) {
			MeshRenderer* meshRenderer = GetComponentType(giant, MeshRenderer);
			for each (auto &material in meshRenderer->GetMaterials()) {
				material->SetBumpScaling(1.0f);
				material->SetReflectivity(0.1f);
			}			
		}

		SceneObject* plane = SceneObject::FindSceneObjectById("Plane");
		if(plane) {
			MeshRenderer* meshRenderer = GetComponentType(plane, MeshRenderer);
			for each (auto &material in meshRenderer->GetMaterials()) {
				material->SetReflectivity(1.0f);
			}
		}
	}

	virtual void Release() {}
};
