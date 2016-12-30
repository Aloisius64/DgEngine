////////////////////////////////////////////////////////////////////////////////
// Filename: Scene_Life2Di.h
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

#pragma region OPENCAL
// declare CA, substate and simulation objects
struct CALModel2D* life2Di;
struct CALSubstate2Di* life2Di_substate;
struct CALRun2D* life2Di_simulation;

// The cell's transition function
void life2DiTransitionFunction(struct CALModel2D* life, int i, int j) {
	int sum = 0, n;
	for(n = 1; n<life->sizeof_X; n++)
		sum += calGetX2Di(life, life2Di_substate, i, j, n);

	if((sum==3)||(sum==2&&calGet2Di(life, life2Di_substate, i, j)==1))
		calSet2Di(life, life2Di_substate, i, j, 1);
	else
		calSet2Di(life, life2Di_substate, i, j, 0);
}

#pragma endregion

class Scene_Life2Di : public ApplicationScene {
public:
	Scene_Life2Di(string nameScene)
		: ApplicationScene(nameScene) {}
	virtual ~Scene_Life2Di() {}

	virtual bool Initialize() {
#pragma region OpenCAL
		// define of the life CA and life_simulation simulation objects
		life2Di = calCADef2D(50, 50, CAL_MOORE_NEIGHBORHOOD_2D, CAL_SPACE_TOROIDAL, CAL_NO_OPT);
		life2Di_simulation = calRunDef2D(life2Di, 1, 0, CAL_UPDATE_IMPLICIT);

		// add the Q substate to the life CA
		life2Di_substate = calAddSubstate2Di(life2Di);

		// add transition function's elementary process
		calAddElementaryProcess2D(life2Di, life2DiTransitionFunction);

		// set the whole substate to 0
		calInitSubstate2Di(life2Di, life2Di_substate, 0);

		// set a glider
		calInit2Di(life2Di, life2Di_substate, 0, 2, 1);
		calInit2Di(life2Di, life2Di_substate, 1, 0, 1);
		calInit2Di(life2Di, life2Di_substate, 1, 2, 1);
		calInit2Di(life2Di, life2Di_substate, 2, 1, 1);
		calInit2Di(life2Di, life2Di_substate, 2, 2, 1);
#pragma endregion

		// Initialize a default camera.
		Camera::InitializeCamera("Camera");

		// Initialize a default directional light.
		Light::InitializeDefaultDirectionalLight();

		CellularAutomataDesc lifeDesc;
		lifeDesc.model2D = life2Di;
		lifeDesc.run2D = life2Di_simulation;
		lifeDesc.id = "LifeCA";
		lifeDesc.tag = "CA";
		lifeDesc.renderLayer = 0;
		lifeDesc.cellSize = 1.0f;
		//lifeDesc.renderStep = 1;
		lifeDesc.drawMode = eDrawMode::DRAWMODE_2D_FLAT;

		// Defining a graphics cellular automata object.
		CellularAutomata* cellularAutomata = CellularAutomata::DefineCellularAutomata(lifeDesc);

		MaterialDesc greyMaterial;
		greyMaterial.materialName = "Grey.Material";
		//greyMaterial.wireframe = true;
		greyMaterial.colorDiffuse = vec4(0.5f, 0.5f, 0.5f, 1.0f);

		// Add substates to the scene.
		SubstateRendererDesc lifeSubstateDesc;
		lifeSubstateDesc.substateName = "Life";
		lifeSubstateDesc.parentObject = "";
		lifeSubstateDesc.cellularAutomata = cellularAutomata;
		lifeSubstateDesc.substate2Di = life2Di_substate;
		lifeSubstateDesc.dataType = eDataType::DYNAMIC_DATA;
		lifeSubstateDesc.materialDesc = &greyMaterial;
		//lifeSubstateDesc.noByteData = ;
		lifeSubstateDesc.colorMinValue = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		lifeSubstateDesc.colorMaxValue = vec4(1.0f, 1.0f, 1.0f, 1.0f);

		SubstateRenderer::DefineSubstate(lifeSubstateDesc);

		return true;
	}

	virtual void LoadBehaviourScripts() {
		SceneObject::AddScriptToSceneObject("Camera", new CameraFPSControllerScript());
		SceneObject::AddScriptToSceneObject("Camera", new FPSRendererScript());
	}

};
