////////////////////////////////////////////////////////////////////////////////
// Filename: Scene_Wave.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <stdlib.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "ApplicationScene.h"
#include "Camera.h"
#include "CellularAutomata.h"
#include "Common.h"
#include "Configuration.h"
#include "DataManager.h"
#include "Gizmo.h"
#include "Light.h"
#include "OpenCALFiles.h"
#include "Shader.h"
#include "SceneObject.h"
#include "System.h"
#include "SubstateRenderer.h"
#include "Transform.h"
#include "UI.h"
#include "Utils.h"
/*Scripts*/
#include "CameraFPSControllerScript.h"
#include "FPSRendererScript.h"

////////////////
// NAMESPACES //
////////////////
using namespace dg;


#pragma region OpenCAL

#define WAVE_ROWS 200
#define WAVE_COLUMNS 200
#define WAVE_STEPS 800
//#define SMOOTH_WAVE

struct CALModel2D* wave;
struct CALRun2D* wave_simulation;
struct CALSubstate2Dr *Q_old;
struct CALSubstate2Dr *Q_old_2_steps;
CALreal max_height = 100.0f, min_height = -100.0f;

CALreal computeCell(struct CALModel2D* wave, int i, int j) {
	CALreal value = 0.0f;

	if(!(i==0||j==0||i==WAVE_ROWS-1||j==WAVE_COLUMNS-1)) {
		value = 2*calGet2Dr(wave, Q_old, i, j)
			-calGet2Dr(wave, Q_old_2_steps, i, j)
			+(calGetX2Dr(wave, Q_old, i, j, 1)
			+calGetX2Dr(wave, Q_old, i, j, 4)
			+calGetX2Dr(wave, Q_old, i, j, 3)
			+calGetX2Dr(wave, Q_old, i, j, 2)
			-4*calGet2Dr(wave, Q_old, i, j))/4;
	}

	return value;
}

void wave_transition_function(struct CALModel2D* wave, int i, int j) {
	CALreal height = 0.0f;
	CALreal height_sum = 0.0f;

	if(!(i==0||j==0||i==WAVE_ROWS-1||j==WAVE_COLUMNS-1)) {
#ifndef SMOOTH_WAVE
		height = computeCell(wave, i, j);
		calSet2Dr(wave, Q_old_2_steps, i, j, calGet2Dr(wave, Q_old, i, j));
		calSet2Dr(wave, Q_old, i, j, height);
#else
		int num = 0;
		CALreal v1 = computeCell(wave, i, j);
		if(v1!=0) {
			height_sum += v1;
			num++;
		}

		CALreal v2 = computeCell(wave, i+1, j);
		if(v2!=0) {
			height_sum += v2;
			num++;
		}

		CALreal v3 = computeCell(wave, i-1, j);
		if(v3!=0) {
			height_sum += v3;
			num++;
		}

		CALreal v4 = computeCell(wave, i, j+1);
		if(v4!=0) {
			height_sum += v4;
			num++;
		}

		CALreal v5 = computeCell(wave, i, j-1);
		if(v5!=0) {
			height_sum += v5;
			num++;
		}

		if(num>0) {
			calSet2Dr(wave, Q_old_2_steps, i, j, calGet2Dr(wave, Q_old, i, j));
			calSet2Dr(wave, Q_old, i, j, height_sum/num);
		}
#endif
	}
}

void wave_init(struct CALModel2D* wave) {}

void wave_steering(struct CALModel2D* wave) {}

void wave_finalize(struct CALModel2D* wave) {}

CALbyte wave_stop_condition(struct CALModel2D* model) {
	if(wave_simulation->step>=WAVE_STEPS)
		return CAL_TRUE;
	return CAL_FALSE;
}

#pragma endregion

class Scene_Wave : public ApplicationScene {
public:
	Scene_Wave(string nameScene)
		: ApplicationScene(nameScene) {}
	virtual ~Scene_Wave() {}

	virtual bool Initialize() {
		// OpenCAL cellular automata initialization.
		//cadef and rundef
		wave = calCADef2D(WAVE_ROWS, WAVE_COLUMNS, CAL_CUSTOM_NEIGHBORHOOD_2D, CAL_SPACE_TOROIDAL, CAL_NO_OPT);
		wave_simulation = calRunDef2D(wave, 1, CAL_RUN_LOOP, CAL_UPDATE_IMPLICIT);

		//add transition function's elementary processes.
		calAddElementaryProcess2D(wave, wave_transition_function);

		//add substates
		Q_old = calAddSubstate2Dr(wave);
		Q_old_2_steps = calAddSubstate2Dr(wave);

		//add neighbors of the Moore neighborhood
		calAddNeighbor2D(wave, 0, 0);	//this is the neighbor 0 (central cell)
		calAddNeighbor2D(wave, -1, 0);	//this is the neighbor 1
		calAddNeighbor2D(wave, 0, -1);	//this is the neighbor 2
		calAddNeighbor2D(wave, 0, +1);	//this is the neighbor 3
		calAddNeighbor2D(wave, +1, 0);	//this is the neighbor 4
		calAddNeighbor2D(wave, -1, -1);	//this is the neighbor 5
		calAddNeighbor2D(wave, +1, -1);	//this is the neighbor 6
		calAddNeighbor2D(wave, +1, +1);	//this is the neighbor 7
		calAddNeighbor2D(wave, -1, +1);	//this is the neighbor 8

		//simulation run
		calRunAddInitFunc2D(wave_simulation, wave_init);
		calRunInitSimulation2D(wave_simulation);
		calRunAddSteeringFunc2D(wave_simulation, wave_steering);
		calRunAddFinalizeFunc2D(wave_simulation, wave_finalize);
		calRunAddStopConditionFunc2D(wave_simulation, wave_stop_condition);

		//setting data
		calInitSubstate2Dr(wave, Q_old, 1.0f);
		calInitSubstate2Dr(wave, Q_old_2_steps, 1.0f);
		calSet2Dr(wave, Q_old, WAVE_ROWS/2, WAVE_COLUMNS/2, max_height);
		calSet2Dr(wave, Q_old, 1+WAVE_ROWS/2, WAVE_COLUMNS/2, max_height);
		calSet2Dr(wave, Q_old, (-1)+WAVE_ROWS/2, WAVE_COLUMNS/2, max_height);
		calSet2Dr(wave, Q_old, WAVE_ROWS/2, 1+WAVE_COLUMNS/2, max_height);
		calSet2Dr(wave, Q_old, WAVE_ROWS/2, (-1)+WAVE_COLUMNS/2, max_height);
		calUpdate2D(wave);

		// Initialize a default camera.
		Camera::InitializeCamera("Camera");

		// Initialize a default directional light.
		Light::InitializeDefaultDirectionalLight();
		SceneObject* lightObject = SceneObject::FindSceneObjectById("MainLight");
		Transform* transform = GetComponentType(lightObject, Transform);
		transform->SetPosition(0.0f, 8.0f, 38.0f);
		Light* lightComponent = GetComponentType(lightObject, Light);
		//lightComponent->ChangeType(eLightType::POINT_LIGHT);
		lightComponent->SetSpecularStrength(1.0f);

		CellularAutomataDesc waveDesc;
		waveDesc.model2D = wave;
		waveDesc.run2D = wave_simulation;
		waveDesc.id = "Wave";
		waveDesc.tag = "CA";
		waveDesc.renderLayer = 0;
		waveDesc.cellSize = 5.0f;
		waveDesc.drawMode = eDrawMode::DRAWMODE_2D_SURFACE;

		// Defining a graphics cellular automata object.
		CellularAutomata* cellularAutomata = CellularAutomata::DefineCellularAutomata(waveDesc);
		transform = GetComponentType(cellularAutomata, Transform);
		transform->SetPosition(0.0f, -50.0f, -200.0f);
		transform->SetRotation(60.0f, 0.0f, 0.0f);

		TextureDesc waterTextureDesc;
		waterTextureDesc.filePath = "./Resources/Textures/water22.png";

		MaterialDesc waterMaterialDesc;
		waterMaterialDesc.materialName = "Water.Material";
		//waterMaterialDesc.wireframe = true;
		waterMaterialDesc.colorAmbient = vec4(0.0f, 97.0f/255.0f, 1.0f, 1.0f);
		waterMaterialDesc.colorDiffuse = vec4(96.0f/255.0f, 205.0f/255.0f, 241.0f/255.0f, 1.0f);
		waterMaterialDesc.shininess = 256.0f;
		waterMaterialDesc.reflectivity = 1.0f;
		waterMaterialDesc.refraction = 1.53f;
		waterMaterialDesc.textureDesc[eTextureType::DIFFUSE_TEXTURE] = waterTextureDesc;

		// Add substates to the scene.
		SubstateRendererDesc waterSubstateDesc;
		waterSubstateDesc.substateName = "WaterSubstate";
		waterSubstateDesc.parentObject = "";
		waterSubstateDesc.cellularAutomata = cellularAutomata;
		waterSubstateDesc.substate2Dr = Q_old;
		waterSubstateDesc.dataType = eDataType::DYNAMIC_DATA;
		waterSubstateDesc.materialDesc = &waterMaterialDesc;
		waterSubstateDesc.noRealData = -9999.9;
		waterSubstateDesc.colorMinValue = vec4(0.0f, 97.0f/255.0f, 1.0f, 1.0f);
		waterSubstateDesc.colorMaxValue = vec4(0.0f, 193.0f/255.0f, 206.0f/255.0f, 1.0f);

		SubstateRenderer::DefineSubstate(waterSubstateDesc);

		// Set the dimension of the grid.
		Gizmo::SetGridSize(50);

		// Skybox.
		string xPos = string("./Resources/SkyBox2/right.jpg");
		string xNeg = string("./Resources/SkyBox2/left.jpg");
		string yPos = string("./Resources/SkyBox2/top.jpg");
		string yNeg = string("./Resources/SkyBox2/bottom.jpg");
		string zPos = string("./Resources/SkyBox2/front.jpg");
		string zNeg = string("./Resources/SkyBox2/back.jpg");
		if(!Environment::Instance()->InitializeSkyBox(xPos, xNeg, yPos, yNeg, zPos, zNeg)) {
			DebugConsoleMessage("Error to initialize Skybox\n");
		}

		return true;
	}

	virtual void LoadBehaviourScripts() {
		SceneObject::AddScriptToSceneObject("Camera", new CameraFPSControllerScript());
		SceneObject::AddScriptToSceneObject("Camera", new FPSRendererScript());
	}

};
