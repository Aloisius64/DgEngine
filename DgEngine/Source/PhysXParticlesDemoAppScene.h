//#pragma once
//
//#include <DgAppScene.h>
//#include <CharacterControllerMove.h>
//#include <CameraFPSControllerScript.h>
//#include <TrackingCameraScript.h>
//#include "ChangeSceneScript.h"
//
//class PhysXParticlesDemoAppScene : public DgAppScene {
//public:
//	PhysXParticlesDemoAppScene() {}
//	PhysXParticlesDemoAppScene(string nameScene)
//		: DgAppScene(nameScene, "./Resources/XmlConfigurationFiles/PhysXParticlesGraphScene.xml") {}
//	virtual ~PhysXParticlesDemoAppScene() {}
//	
//	virtual void behaviourScriptCallback() {
//		SceneObject::addScriptToGameObject("CameraController", new CameraFPSControllerScript());
//		SceneObject::addScriptToGameObject("CameraController", new ChangeSceneScript());
//	}
//};
